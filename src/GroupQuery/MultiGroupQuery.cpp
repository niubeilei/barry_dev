////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
// 
// Modification History:
// 2014/10/16  Shawn Li
////////////////////////////////////////////////////////////////////////////
#include "GroupQuery/MultiGroupQuery.h"

#include "BitmapMgr/BitmapUtil.h"
#include "BitmapTrans/Ptrs.h"
#include "BitmapTrans/TransBitmapQuery.h"
#include "Debug/Except.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadPool.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("multigroupquery", __FILE__, __LINE__);
//static bool sgShowLog = false;


AosMultiGroupQuery::AosMultiGroupQuery(
		const AosRundataPtr &rdata)
:
mRundata(rdata),
mLock(OmnNew OmnMutex()),
mQueryType(eNoGroup),
mNoGroupQueried(false)
{
}


AosMultiGroupQuery::~AosMultiGroupQuery()
{
}



bool 
AosMultiGroupQuery::addCond(
			const OmnString &iilname,
			const AosOpr	&opr,
			const OmnString	&value1,
			const OmnString	&value2)
{
	mIILNames.push_back(iilname);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setBlockSize(0);
	query_context->setOpr(opr);
	query_context->setStrValue(value1);
	if(value2 != "")
	{
		query_context->setStrValue2(value2);
	}
	mQueryContexts.push_back(query_context);
	return true;
}

bool 
AosMultiGroupQuery::addCond(
				const OmnString &iilname,
				const AosQueryContextObjPtr &context)
{
	mIILNames.push_back(iilname);
	context->setBlockSize(0);
	mQueryContexts.push_back(context);
	return true;
}

bool 
AosMultiGroupQuery::setGroupFields(
				const OmnString &iilname_f1,
				const bool &f1_reverse,
				const OmnString &iilname_f2,
				const bool &f2_reverse)
{
	mF1IILName = iilname_f1;
	mF2IILName = iilname_f2;
	if(!mF1QueryContext)mF1QueryContext = AosQueryContextObj::createQueryContextStatic();
	if(!mF2QueryContext)mF2QueryContext = AosQueryContextObj::createQueryContextStatic();
	mF1Reverse = f1_reverse;
	mF2Reverse = f2_reverse;

	mF1QueryContext->setOpr(eAosOpr_distinct);
	mF1QueryContext->setReverse(mF1Reverse);
	mF2QueryContext->setOpr(eAosOpr_distinct);	
	mF2QueryContext->setReverse(mF2Reverse);
	mQueryType = eMultiGroup;
	return true;
}

bool 
AosMultiGroupQuery::setGroupFields(
				const OmnString &iilname_f2,
				const bool &f2_reverse)
{
	mF2IILName = iilname_f2;
	if(!mF2QueryContext)mF2QueryContext = AosQueryContextObj::createQueryContextStatic();

	mF2QueryContext->setOpr(eAosOpr_distinct);	
	mF2Reverse = f2_reverse;
	mF2QueryContext->setReverse(mF2Reverse);
	mQueryType = eSingleGroup;
	return true;
}

bool 
AosMultiGroupQuery::nextBlock(AosBitmapObjPtr &bitmap)
{
	if(mQueryType == eSingleGroup)
	{
		if(mPageSize > 100000000000ULL)
		{
			OmnScreen << "mPageSize too big: " << mPageSize << endl;
			mPageSize = 100000000000ULL;
		}
		mF2QueryContext->setBlockSize(mPageSize);
	}
	if(mQueryType == eMultiGroup)
	{
		if(mPageSize > 100000000000ULL)
		{
			OmnScreen << "mPageSize too big: " << mPageSize << endl;
			mPageSize = 100000000000ULL;
		}
		mF2QueryContext->setBlockSize(mPageSize);
		mF1QueryContext->setBlockSize(1);
	}
	if((mQueryType == eNoGroup) && mNoGroupQueried)
	{
		// queried
		bitmap = 0;
		return true;
	}
  
	if(mQueryRslts.size() < mIILNames.size())
	{
		mQueryRslts.resize(mIILNames.size());
		mBitmaps.resize(mIILNames.size());
	}

	bool rslt = true;
	bool finished = false;
	u64 new_page_size = mPageSize;
	u64 scanned_all = 0;

	AosBitmapObjPtr rslt_bmp = AosGetBitmap();
	AosBitmapObjPtr cur_bmp;
	while(rslt && !finished)
	{
		scanned_all += new_page_size;
		cur_bmp = AosGetBitmap();
		rslt = nextBlock1(cur_bmp,finished);
		if(!cur_bmp)
		{
			if(rslt_bmp->getNumBits())
			{
				bitmap = rslt_bmp;
			}
			else
			{
				bitmap = 0;
			}
			// finished
			return true;
		}
		
		// adjust page size
		if(new_page_size > 0 && (mQueryType == eSingleGroup || mQueryType == eMultiGroup))
		{
			u64 num_rslt = 0;
			if(cur_bmp)num_rslt = cur_bmp->getNumBits();
			aos_assert_r(rslt_bmp,false);
			u64 num_all = rslt_bmp->getNumBits() + num_rslt;
			// if num_rslt == 0, page_size *= 10
			if(num_rslt == 0)
			{
				new_page_size *= 10;
			}
			else if(num_all < mPageSize)
			{
				// calculate density, get 1.5 by density
				aos_assert_r(num_all,false);
				new_page_size = (mPageSize - num_all) * 3 * scanned_all / 2 / num_all;
			}
			if(new_page_size < mPageSize)
			{
				new_page_size = mPageSize;
			}
			//
			//
			//
			if(new_page_size > 100000000000ULL)
			{
				OmnScreen << "new_page_size too big: " << new_page_size << endl;
				new_page_size = 100000000000ULL;
			}
			mF2QueryContext->setBlockSize(new_page_size); 
		}
							
		if((cur_bmp && cur_bmp->getNumBits() > 0) || finished)
		{
			// got something
			rslt_bmp->orBitmap(cur_bmp);
			if(rslt_bmp->getNumBits() >= (i64)mPageSize || finished)
			{
				bitmap = rslt_bmp;
				return true;
			}
		}
		// got nothing this turn
	}
	
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosMultiGroupQuery::nextBlock1(AosBitmapObjPtr &bitmap,bool &finished)
{
	finished = false;
	// phase 1, bitmap query for all the conditions and f1, f2
	bool rslt = false;
	vector<OmnThrdShellProcPtr> runners;
    OmnThrdShellProcPtr runner;
	AosMultiGroupQueryPtr thisPtr(this,false);

	bool first_query = false;
	
	if(mQueryType == eNoGroup)
	{
		finished = true;	
		first_query = true;
		mNoGroupQueried = true;
	}
	else if (mQueryType == eSingleGroup)
	{
		// case 1. new start
		// case 2. F2 has not finished
		// case 3. F2 finished
		AosMultiGroupQueryPtr thisPtr(this,false);
		if(mF2QueryContext->finished())
		{
			bitmap = 0;
			finished = true;
			return true;
		}
		else
		{
			if(mF2QueryContext->getCrtValue() == "")
			{
				first_query = true;
			}
			// case 2. F2 has not finished
		    runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mF2IILName, mF2QueryContext, 2, 0);
		    runners.push_back(runner);		
		}
	}
	else if (mQueryType == eMultiGroup)
	{
		// case 1. new start
		// case 2. F2 has not finished
		// case 3. F2 finished
		if(mF1QueryContext->getCrtValue() == "" && !mF1QueryContext->finished())
		{
			// case 1. new start
			first_query = true;
			runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mF1IILName, mF1QueryContext, 1, 0);
		    runners.push_back(runner);
		
		    runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mF2IILName, mF2QueryContext, 2, 0);
		    runners.push_back(runner);
		}
		else if(mF2QueryContext->finished())
		{
			// case 3. F2 finished
			if(mF1QueryContext->finished())
			{
				// all finished
				bitmap = 0;
				return true;
			}
			runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mF1IILName, mF1QueryContext, 1, 0);
		    runners.push_back(runner);
	
			mF2QueryContext->setFinished(false);
			mF2QueryContext->setCrtValue("");
			
			u64 sz = mF1QueryContext->getBlockSize();
			sz *= 10;
			if(sz > 100000000000ULL) sz = 100000000000ULL;
			mF1QueryContext->setBlockSize(sz);	
			
		    runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mF2IILName, mF2QueryContext, 2, 0);
		    runners.push_back(runner);
		}
		else
		{
			// case 2. F2 has not finished
		    runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mF2IILName, mF2QueryContext, 2, 0);
		    runners.push_back(runner);		
		}
	}

	if(first_query)
	{
		for (u32 i=0; i<mIILNames.size(); i++)
		{
		    runner = OmnNew AosMultiGroupQueryPhase1Thrd(mRundata, thisPtr, mIILNames[i], mQueryContexts[i], 0, i);
		    runners.push_back(runner);
		}
	}	

	OmnThreadPool::runProcSync(runners);       

	runners.clear();

	if(mQueryType == eSingleGroup)
	{
		if(mF2QueryContext->finished())
		{
			finished = true;
		}
	}
	if(mQueryType == eMultiGroup)
	{
		if(mF2QueryContext->finished() && mF1QueryContext->finished())
		{
			finished = true;
		}
	}



	// phase 2
	if(!mF1Bitmap)
	{
		mF1Bitmap = AosGetBitmap();
	}
	if(!mF2Bitmap)
	{
		mF2Bitmap = AosGetBitmap();
	}
	for(u32 i = 0;i < mIILNames.size();i++)
	{
		if(!mBitmaps[i])
		{
			mBitmaps[i] = AosGetBitmap();
		}
	}

	mRsltBitmap = AosGetBitmap();
		
	// set iil names and 
	vector<OmnString> all_iilnames;
	for(u32 i = 0;i < mIILNames.size();i++)
	{
		all_iilnames.push_back(mIILNames[i]);
	}
	
	if (mQueryType == eNoGroup)
	{
	}
	else if (mQueryType == eSingleGroup)
	{
		all_iilnames.push_back(mF2IILName);
	}
	else if (mQueryType == eMultiGroup)
	{
		all_iilnames.push_back(mF1IILName);
		all_iilnames.push_back(mF2IILName);
	}

	vector<AosQueryRsltObjPtr> bitmap_idlists = mQueryRslts;

	if (mQueryType == eNoGroup)
	{
	}
	else if (mQueryType == eSingleGroup)
	{
		bitmap_idlists.push_back(mF2QueryRslt);
	}
	else if (mQueryType == eMultiGroup)
	{
		bitmap_idlists.push_back(mF1QueryRslt);
		bitmap_idlists.push_back(mF2QueryRslt);
	}
	
	bool is_local = true;
	for(u32 i = 0;i < bitmap_idlists.size();i++)
	{
		if(bitmap_idlists[i] && bitmap_idlists[i]->getNumDocs() > 0)
		{
			is_local = false;
			break;
		}
	}
	
	if(is_local)
	{
		bool init = false;
		mRsltBitmap = AosGetBitmap();
	
		for(u32 i = 0;i < mIILNames.size();i++)
		{
			if(!init)
			{
				init = true;
				mRsltBitmap = mBitmaps[i]->clone(mRundata);
			}
			else
			{
				mRsltBitmap->andBitmap(mBitmaps[i]);
			}
		}


		if (mQueryType == eNoGroup)
		{
			aos_assert_r(mIILNames.size() > 0,false);
		}
		else if (mQueryType == eSingleGroup)
		{
			if(!init)
			{
				init = true;
				mRsltBitmap = mF2Bitmap->clone(mRundata);
			}
			else
			{
				mRsltBitmap->andBitmap(mF2Bitmap);
			}
		}
		else if (mQueryType == eMultiGroup)
		{
			if(!init)
			{
				init = true;
				mRsltBitmap = mF1Bitmap->clone(mRundata);
			}
			else
			{
				mRsltBitmap->andBitmap(mF1Bitmap);
			}

			if(!init)
			{
				init = true;
				mRsltBitmap = mF2Bitmap->clone(mRundata);
			}
			else
			{
				mRsltBitmap->andBitmap(mF2Bitmap);
			}
		}		
		bitmap = mRsltBitmap;
		mRsltBitmap = 0;	
		return true;
	}

	
	// split all the bitmaps for cubes
	int num_cubes = AosGetNumCubes();
	aos_assert_r(num_cubes > 0, false);
	vector<AosBitmapObjPtr> partial_bitmaps[num_cubes];
	
	vector<AosBitmapObjPtr> cur_bitmaps;
	
	for(u32 i = 0;i < mIILNames.size();i++)
	{
		rslt = AosBitmapUtil::splitBitmapToCubes(mRundata, mBitmaps[i], cur_bitmaps);
		aos_assert_r(rslt,false);
		for(int j = 0;j < num_cubes;j++)
		{
			partial_bitmaps[j].push_back(cur_bitmaps[j]);
		}
		cur_bitmaps.clear();
	}


	if (mQueryType == eNoGroup)
	{
	}
	else if (mQueryType == eSingleGroup)
	{
		rslt = AosBitmapUtil::splitBitmapToCubes(mRundata, mF2Bitmap, cur_bitmaps);
		aos_assert_r(rslt,false);
		for(int j = 0;j < num_cubes;j++)
		{
			partial_bitmaps[j].push_back(cur_bitmaps[j]);
		}
		cur_bitmaps.clear();
	}
	else if (mQueryType == eMultiGroup)
	{
		rslt = AosBitmapUtil::splitBitmapToCubes(mRundata, mF1Bitmap, cur_bitmaps);
		aos_assert_r(rslt,false);
		for(int j = 0;j < num_cubes;j++)
		{
			partial_bitmaps[j].push_back(cur_bitmaps[j]);
		}
		cur_bitmaps.clear();
	
		rslt = AosBitmapUtil::splitBitmapToCubes(mRundata, mF2Bitmap, cur_bitmaps);
		aos_assert_r(rslt,false);
		for(int j = 0;j < num_cubes;j++)
		{
			partial_bitmaps[j].push_back(cur_bitmaps[j]);
		}
		cur_bitmaps.clear();
	}



	for(int j = 0;j < num_cubes;j++)
	{
	    runner = OmnNew AosMultiGroupQueryPhase2Thrd(mRundata, thisPtr, 
	    	j, all_iilnames, bitmap_idlists, partial_bitmaps[j]); 
    	runners.push_back(runner);
	}    
	OmnThreadPool::runProcSync(runners);
	runners.clear();

	bitmap = mRsltBitmap;
	mRsltBitmap = 0;	
	return true;
}

bool 
AosMultiGroupQuery::setPhase1Rslt(
	const AosQueryRsltObjPtr &query_rslt,
	const AosBitmapObjPtr &query_bitmap,
	const int key_order,
	const int cond_order)
{
	mLock->lock();
	if(key_order == 0)
	{
		mQueryRslts[cond_order] = query_rslt;
		mBitmaps[cond_order] = query_bitmap;
	}
	
	if(key_order == 1)
	{
		mF1QueryRslt = query_rslt;
		mF1Bitmap = query_bitmap;
	}

	if(key_order == 2)
	{
		mF2QueryRslt = query_rslt;
		mF2Bitmap = query_bitmap;
	}

	mLock->unlock();

	return true;
}

bool 
AosMultiGroupQuery::setPhase2Rslt(const AosBitmapObjPtr &query_bitmap)
{
	mLock->lock();
	if(query_bitmap)
	{
		mRsltBitmap->orBitmap(query_bitmap);
	}
	mLock->unlock();
	return true;
}

AosGroupQueryObjPtr 
AosMultiGroupQuery::createQuery(const AosRundataPtr &rdata) const
{
	return OmnNew AosMultiGroupQuery(rdata); 
}

bool
AosMultiGroupQueryPhase1Thrd::run()
{
	AosBitmapObjPtr 	query_bitmap = AosGetBitmap();
	AosQueryRsltObjPtr	query_rslt   = AosQueryRsltObj::getQueryRsltStatic();
	//bool rslt = AosIILClient::getSelf()->bitmapQueryNewSafe(
	bool rslt = AosIILClientObj::getIILClient()->bitmapQueryNewSafe(
				mIILName, query_rslt, query_bitmap, mQueryContext, mRundata);

	OmnScreen << "group bitmap query :" << mIILName << ":" << rslt << ":"
		<< query_rslt->getNumDocs() << ":" << query_bitmap->getNumBits() << endl;

	if (!rslt)
	{
		return mQuery->setPhase1Rslt(0, 0, mKeyOrder, mCondOrder);
	}
	mQuery->setPhase1Rslt(query_rslt, query_bitmap, mKeyOrder, mCondOrder);
	return true;
}

bool
AosMultiGroupQueryPhase1Thrd::procFinished()
{
	return true;
}


bool
AosMultiGroupQueryPhase2Thrd::run()
{
	vector<u32> empty_list;
	AosTransBitmapQueryPtr trans = OmnNew AosTransBitmapQuery(
			mRundata, 
			mCubeID, 
			9999,// mTermID 
			mIILNames, 
			mBitmapIDLists,//	vector<AosQueryRsltObjPtr> bitmap_idlists; 
			mPartialBitmaps, 
			empty_list);
	AosSendTransSyncResp(mRundata, trans);
	AosBuffPtr resp = trans->getResp();
	aos_assert_r(resp,false);
	// parse 

	resp->getU64(0);//and_term_id 
	resp->getU32(0);// error_flag
	AosBitmapObjPtr query_bitmap;
	OmnString errmsg = resp->getOmnStr("");
	aos_assert_r(errmsg == "",false); 
	u64 bitmap_flag = resp->getU32(0);
	if(bitmap_flag)
	{
	    u32 len = resp->getU32(0);
	    if(len > 0)
	    {
	        AosBuffPtr bb = resp->getBuff(len, false AosMemoryCheckerArgs);
	        query_bitmap = AosGetBitmap();
	        query_bitmap->loadFromBuff(bb);
	    } 
	}

	mQuery->setPhase2Rslt(query_bitmap);
	return true;
}

bool
AosMultiGroupQueryPhase2Thrd::procFinished()
{
	return true;
}







