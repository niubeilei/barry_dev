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
// mQueriedStart:	This is the entry start position to fetch
// mPageSize:		This is the size of the page to fetch
// mBlockStart:		This is the block start position
// mBlockSize:		This is the block size
// mEntryCursor:	This is the next entry position. If no start position
// 					is specified, this serves as the next page start position.
// mDensity:		This is the current density. -1 means no density yet.
//
// Modification History:
// 2013/12/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "QueryCacher/QueryCacher.h"

#include "API/AosApi.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryProcObj.h"
#include "SEInterfaces/QueryReqObj.h"


// AosGetConfig(rdata, AOSCONFIG_QUERY_DEFAULT_PAGE_SIZE, 20);
static AosXmlTagPtr sgDefaultQuery;

AosQueryCacher::AosQueryCacher()
{
}


AosQueryCacher::~AosQueryCacher()
{
}


bool 
AosQueryCacher::runQuery(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &query_req, 
		const AosQueryProcCallbackPtr &callback)
{
	// This is the start point to query data. 
	aos_assert_rr(query_req, rdata, false);
	aos_assert_rr(callback, rdata, false);
	mCallback = callback;
	mQueryReq = query_req;

	mQueryProc = query_req->getQueryProc();
	if (!mQueryProc)
	{
		AosJimoPtr jimo;

		AosXmlTagPtr orig_req = query_req->getOrigReq();
		if (orig_req)
		{
			jimo = AosCreateJimo(rdata, orig_req);
		}

		if (!jimo)
		{
			if (!sgDefaultQuery)
			{
				createDefaultQuery(rdata);
			}
			aos_assert_rr(sgDefaultQuery, rdata, false);
			jimo = AosCreateJimo(rdata, 0, sgDefaultQuery, "");
		}

		if (!jimo)
		{
			// jimo = AosQueryProcObj::getObject();
			// aos_assert_rr(jimo, rdata, false);
			// jimo = jimo->cloneJimo();
			AosSetErrorUser(rdata, "failed_creating_jimo") << query_req->toString() << enderr;
			return false;
		}

		if (jimo->getJimoType() != AosJimoType::eQueryProc)
		{
			AosSetErrorUser(rdata, "incorrect_jimo") << query_req->toString() << enderr;
			return false;
		}

		mQueryProc = dynamic_cast<AosQueryProcObj*>(jimo.getPtr());
		aos_assert_r(mQueryProc, false);
		query_req->setQueryProc(mQueryProc);
	}
	
	mQueriedStart = query_req->getStartPos();
	mPageSize = query_req->getPageSize();
	if (mPageSize < 0) mPageSize = smDefaultPageSize;
	if (mPageSize > smMaxPageSize) mPageSize = smMaxPageSize;

	// Query starts in one of the three scenarios:
	// 1. No start position specified: retrieve the next page
	// 2. Some data being cached: move to a page
	// 3. No cached data and wants to go to a specific location: create the density first
	if (mQueriedStart < 0) return retrieveNextPage(rdata);
	if (mCachers.size() > 0) return moveToPage(rdata);
	return createDensity(rdata);
}


bool
AosQueryCacher::retrieveNextPage(const AosRundataPtr &rdata)
{
	if (mCachers.size() <= 0)
	{
		// This is to retrieve the first page
		mQueriedStart = 0;
		mBlockStart = 0;
		mBlockSize = smInitBlockSize;
		mBlockEnd = -1;
		mStatus = eFetchContents;
		return fetchData(rdata);
	}

	// Some contents were cached. 
	aos_assert_rr(mCrtCacherId >= 0 && (u32)mCrtCacherId < mCachers.size(), rdata, false);
	Cacher &cacher = mCachers[mCrtCacherId];
	cacher.hit();
	AosQueryRsltObjPtr rslts;
	if (cacher.tryOnePage(mQueriedStart, mPageSize, rslts))
	{
		mFullHit++;
		return queryFinishedPriv(rdata, rslts);
	}
	
	// The cacher does not have enough contents (or have any content at all).
	// Need to fetch.
	mBlockStart = cacher.getNextBlockStart();
	mBlockSize = cacher.getNextBlockSize();
	mPartialHit++;
	mBlockEnd = -1;
	mStatus = eFetchContents;
	if ((u32)mCrtCacherId < mCachers.size()-1) mBlockEnd = mCachers[mCrtCacherId+1].mBlockStart;
	return fetchData(rdata);
}


bool
AosQueryCacher::moveToPage(const AosRundataPtr &rdata)
{
	// The density is good enough. The following information is available:
	// mControllingIILSize:		This is the number of raw entries in the
	// 							controlling IIL.
	// mDensity:				The density of the controlling IIL.
	// mCachers:				A number of cachers (possibly zero).
	// mPageSize:
	// 
	// 1. Only one cacher
	//    The cacher must start with Raw Entry 0. It checks whether the cacher
	//    covers any requested contents. 
	aos_assert_rr(mCachers.size() > 0, rdata, false);

	if (mCachers.size() == 1)
	{
		Cacher &cacher = mCachers[0];
		AosQueryRsltObjPtr rslts;
		bool rslt = cacher.tryOnePage(mQueriedStart, mPageSize, rslts);
		aos_assert_r(rslt, false);

		// The requested is available in the cahcer.
		if (rslts)
		{
			mFullHit++;
			cacher.hit();
			return queryFinishedPriv(rdata, rslts);
		}

		if (cacher.containPartialPage(mQueriedStart, mPageSize))
		{
			// It needs to retrieve more contents
			mBlockStart = cacher.getNextBlockStart();
			mBlockSize = cacher.getNextBlockSize(mDensity, mPageSize);
			mBlockEnd = -1;
			mStatus = eFetchContents;
			mPartialHit++;
			cacher.hit();
			return fetchData(rdata);
		}

		// The requested is not in the cache at all. Check whether it is
		// far enough from the current cacher.
		if (cacher.isFarFromRequested(mDensity, mQueriedStart))
		{
			// It is far from the current cacher. Need to create a new cacher.
			return createNewCacher(rdata, mQueriedStart/mDensity);
		}

		// It is not far from the current cacher. Need to retrieve more:
		mBlockStart = cacher.getNextBlockStart();
		mBlockSize = cacher.getNextBlockSize(mDensity, mPageSize);
		mBlockEnd = -1;
		mStatus = eFetchContents;
		mExtensionHit++;
		cacher.hit();
		return fetchData(rdata);
	}

	// 2. There are more than one cacher. Find the closest cacher
	int cacher_id = findClosestCacher(mQueriedStart);
	if (cacher_id < 0)
	{
		// The requested is not close enough to any of the existing
		// cachers. Create a new cacher.
		return createNewCacher(rdata, mQueriedStart/mDensity);
	}
	
	// A cacher is selected to provide the contents.
	// If the cacher is not the last one, check whether the gap 
	// to its next cacher is small enough. If yes, it will fetch all
	// the contents in the gap.
	if (isCacherCloseEnoughToNext(cacher_id, mBlockStart, mBlockSize))
	{
		mStatus = eMergeCachers;
		mBlockEnd = mCachers[mCrtCacherId+1].mBlockStart-1;
		mFetchGapBlockHit++;
		mCrtCacherId = cacher_id;
		return fetchData(rdata);
	}

	// Just extend the current cacher. 
	Cacher &cacher = mCachers[cacher_id]; 
	cacher.hit();
	mBlockStart = cacher.getNextBlockStart();
	mBlockSize = cacher.getNextBlockSize(mDensity, mPageSize);
	mStatus = eFetchContents;
	mPartialHit++;
	return fetchData(rdata);
}


bool
AosQueryCacher::createDensity(const AosRundataPtr &rdata)
{
	// This is similar to fetch the first page. 
	mBlockStart = 0;
	mBlockSize = smCreateDensityBlockSize;
	mBlockEnd = -1;
	mStatus = eCreateDensity;
	createNewCacher(rdata, 0);
	return fetchData(rdata);
}


bool
AosQueryCacher::fetchData(const AosRundataPtr &rdata)
{
	AosQueryCacherObjPtr thisptr(this, false);
	mQueryProc->setBlockStart(mBlockStart);
	mQueryProc->setBlockSize(mBlockSize);
	mQueryProc->setBlockEnd(mBlockEnd);
	return mQueryProc->runQuery(rdata, mQueryReq, thisptr);
}


int 
AosQueryCacher::findClosestCacher(const i64 start_pos)
{
	// This function finds the closest cacher for 'start_pos'. 
	// Each cacher 
	aos_assert_r(mCachers.size() > 0, -1);
	for (u32 i=0; i<mCachers.size(); i++)
	{
		if (mCachers[i].isInCloseRange(start_pos, smCloseEnoughMax)) return i;
	}
	return -1;
}


bool
AosQueryCacher::createNewCacher(
		const i64 block_start, 
		const i64 block_size, 
		const AosQueryRsltObjPtr &results, 
		const bool is_end)
{

	Cacher cacher;
	cacher.mBlockStart = block_start;
	cacher.mBlockSize = block_size;
	cacher.mQueryRslts = results;
	cacher.mIsEOF = is_end;
	insertCacher(cacher);
	return true;
}


bool
AosQueryCacher::removeOneCacher()
{
	int hit = mCachers[0].getHit();
	u32 idx = 0;
	for (u32 i=1; i<mCachers.size()-1; i++)
	{
		if (mCachers[i].getHit() < hit)
		{
			idx = i;
			hit = mCachers[i].getHit();
		}
	}

	if (idx < mCachers.size()-1)
	{
		for (u32 i=idx; i<mCachers.size()-1; i++)
		{
			mCachers[i] = mCachers[i+1];
		}
	}
	mCachers.pop_back();

	return true;
}


bool
AosQueryCacher::insertCacher(const Cacher &cacher)
{
	if (tooManyCachers()) removeOneCacher();

	for (u32 i=0; i<mCachers.size(); i++)
	{
		if (mBlockStart > mCachers[i].mBlockStart)
		{
			mCachers.push_back(cacher);
			if (i < mCachers.size()-1)
			{
				for (u32 k=i; k<mCachers.size()-1; k++)
				{
					mCachers[k+1] = mCachers[k];
				}
			}
			mCachers[i] = cacher;
			mCrtCacherId = i;
			return true;
		}
	}

	mCachers.push_back(cacher);
	return true;
}


bool
AosQueryCacher::createNewCacher(const AosRundataPtr &rdata, const i64 block_start)
{
	// It wants to move to position 'mQueriedStart'. The density
	// is available. 
	mBlockStart = block_start; 
	mBlockSize = smInitBlockSize;
	mBlockEnd = -1;
	mStatus = eFetchContents;
	Cacher cacher;
	cacher.mBlockStart = mBlockStart;
	cacher.mBlockSize = mBlockSize;
	cacher.mEntryStart = mQueriedStart;

	// Find the location to insert the cacher
	insertCacher(cacher);
	return fetchData(rdata);
}


bool
AosQueryCacher::queryFinishedPriv(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &results)
{
	OmnString records;
	bool rslt = mQueryReq->generateResults(rdata, results, records);
	if (!rslt)
	{
		return mCallback->queryFailed(rdata, rdata->getErrmsg());
	}
	return mCallback->queryFinished(rdata, records);
}


bool
AosQueryCacher::isCacherCloseEnoughToNext(
		const int cacher_id, 
		const i64 block_start, 
		const i64 block_size)
{
	aos_assert_r(cacher_id >= 0, false);
	if ((u32)cacher_id == mCachers.size()-1) return false;
	aos_assert_r((u32)cacher_id < mCachers.size()-1, false);
	aos_assert_r(mDensity > 0.0, false);

	// The cacher is close enough if it has less than smMinCacherGap 
	// number of raw entries to the next cacher.
	i64 next_block_start = mCachers[cacher_id+1].mBlockStart;
	return (next_block_start - mCachers[cacher_id].mBlockStart + 
			mCachers[cacher_id].mBlockSize <= smMinCacherGap);
}


bool 
AosQueryCacher::queryFinished(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &results,
		const AosQueryProcObjPtr &proc)
{
	i64 matched_size = proc->getMatchedSize();
	i64 block_start = proc->getBlockStart();
	i64 block_size = proc->getQueryCursor();
	bool is_end = proc->finished();

	// Some results are obtained. 
	i64 num_docs = results->getNumDocs();
	aos_assert_r(num_docs >= 0, false);
	if (num_docs == 0)
	{
		// No contents were found. 
		return queryFinishedPriv(rdata, 0);
	}

	mTotalNumDocsQueried += num_docs;
	mTotalNumRawEntriesTried += block_size;
	mDensity = mTotalNumDocsQueried * 1.0 / mTotalNumRawEntriesTried;
	if (mSortingIILSize < 0) mSortingIILSize = matched_size;
	mTotal = mSortingIILSize * mDensity;

	AosQueryRsltObjPtr rslts;
	switch (mStatus)
	{
	case eFetchContents:
		 if (mCachers.size() <= 0)
		 {
			 createNewCacher(block_start, block_size, results, is_end);
			 mCrtCacherId = 0;
			 aos_assert_r(results->getNumDocs() >= mPageSize, false);
		 	 AosQueryRsltObjPtr rslts;
		  	 if (!mCachers[0].getResults(rslts, mQueriedStart, mPageSize)) return false;
		  	 return queryFinishedPriv(rdata, rslts);
		 }
		 else
		 {
		 	 aos_assert_r(mCrtCacherId >= 0 && (u32)mCrtCacherId < mCachers.size()-1, false);
		  	 Cacher &cacher = mCachers[mCrtCacherId];
		 	 cacher.appendResults(results, block_start, block_size, is_end);
			 AosQueryRsltObjPtr rslts;
		  	 if (!mCachers[0].getResults(rslts, mQueriedStart, mPageSize)) return false;
		  	 return queryFinishedPriv(rdata, rslts);
		 }
		 break;

	case eMergeCachers:
		 aos_assert_r(!is_end, false);
		 aos_assert_r(mCrtCacherId >= 0 && (u32)mCrtCacherId < mCachers.size()-1, false);
		 {
			 Cacher &cacher = mCachers[mCrtCacherId];
			 cacher.appendResults(results, block_start, block_size, is_end);
			 cacher.mergeCacher(mCachers[mCrtCacherId+1]);
			 for (u32 i=mCrtCacherId+1; i<mCachers.size()-1; i++)
			 {
				 mCachers[i] = mCachers[i+1];
			 }
			 mCachers.pop_back();
		 }
		 if (!mCachers[mCrtCacherId].getResults(rslts, mQueriedStart, mPageSize)) return false;
		 return queryFinishedPriv(rdata, rslts);

	case eCreateDensity:
		 aos_assert_r(mCachers.size() == 1, false);
		 mCachers[0].appendResults(results, block_start, block_size, is_end);
		 mCrtCacherId = 0;
		 return moveToPage(rdata.getPtr());

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


AosXmlTagPtr 
AosQueryCacher::createDefaultQuery(const AosRundataPtr &rdata)
{
	OmnString jimostr = "<jimo ";
	jimostr << "current_version=\"version_0\" "
		<< "zky_classname=\"AosBitmapQueryProc\" "
		<< "zky_objid=\"bitmap_query_jimo\">"
		<< "<versions>"
		<< 		"<version_0>libQueryProc.so</version_0>"
		<< "</versions>"
		<< "</jimo>";

	sgDefaultQuery = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgDefaultQuery, 0);
	return sgDefaultQuery;
}


bool
AosQueryCacher::Cacher::tryOnePage(
		i64 &queried_start, 
		int &page_size, 
		AosQueryRsltObjPtr &results)
{
	// 'entry_start' is the start position for the results to retrieve. 
	if (queried_start < 0) queried_start = mCursor;
	aos_assert_r(queried_start >= mEntryStart, false);
	if (queried_start - mEntryStart + page_size + 1 > mNumEntries) 
	{
		if (mIsEOF)
		{
			mCursor = mNumEntries;
			if (queried_start >= mNumEntries)
			{
				results = 0;
				return true;
			}
			results = mQueryRslts->getResults(queried_start - mEntryStart, page_size);
			return true;
		}
		results = 0;
		return true;
	}

	mCursor += page_size;
	results = mQueryRslts->getResults(queried_start - mEntryStart, page_size);
	aos_assert_r(results->getNumDocs() == page_size, false);
	return true;
}


bool 
AosQueryCacher::Cacher::isFarFromRequested(const float density, const i64 entry_start)
{
	if (density == 0) return true;
	
	if (entry_start >= mEntryStart)
	{
		i64 nn = (entry_start - (mEntryStart + mNumEntries))/density;
		if (nn <= smCloseEnoughMax) return false;
		return false;
	}

	i64 nn = (mEntryStart-entry_start)/density;
	if (nn <= smCloseEnoughMax) return false;
	return true;
}


bool
AosQueryCacher::Cacher::isInCloseRange(const i64 pos, const int delta)
{
	if (pos < mBlockStart)
	{
		return (mBlockStart - pos) <= delta;
	}
	return pos <= mBlockStart + mBlockSize + delta;
}
#endif
