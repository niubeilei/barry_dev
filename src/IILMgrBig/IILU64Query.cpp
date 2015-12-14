////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 2012/04/16 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/IILU64.h"

#include "AppMgr/App.h"
#include "BatchQuery/BatchQueryCtlr.h"
#include "IILMgrBig/IILMgr.h"
#include "IILUtil/IILLog.h"
#include "Porting/ThreadDef.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEUtil/Docid.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"


bool
AosIILU64::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->copyFrom(query_rslt);
	bool rslt = queryNewPriv(query_rslt,
							 query_bitmap,
							 query_context,
							 false, 
							 rdata);
	query_context->copyTo(query_rslt);	
	return rslt;
}

bool
AosIILU64::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	if ((int)mNumDocs == 0)
	{
		query_context->setFinished(true);
		return true;
	}
	
	if (query_rslt)
	{
		query_rslt->setDataReverse(false);
	}

	aos_assert_r(!query_context->isCheckRslt(), false);

	// There aretwo special cases to handle:
	//	2. opr == not equal
	AosOpr opr = query_context->getOpr();
	if(opr == eAosOpr_ne)
	{
		return queryNewNEPriv(query_rslt, query_bitmap, query_context, iilmgrlock, rdata);
	}

	// Normal query
	//	1. get the block start/end iilidx
	//	2. copy the data
	
	//1. get the block start/end iilidx
	
	AosIILIdx block_start_iilidx;
	AosIILIdx block_end_iilidx;

	bool has_data = true;
	bool rslt = queryPagingProc(
					query_context, 
					block_start_iilidx, 
					block_end_iilidx, 
					has_data, 
					iilmgrlock, 
					rdata);
	aos_assert_r(rslt, false);

	if (!has_data)
	{
		query_context->setFinished(true);
		return true;
	}

	//2. copy the data
	rslt = copyData(query_rslt,
					query_bitmap,
					query_context,
					block_start_iilidx,
					block_end_iilidx,
					iilmgrlock, 
					rdata);	
	query_context->setTotalDocInIIL(mNumDocs);
	return rslt;
}


bool
AosIILU64::queryNewNEPriv(
		const AosQueryRsltObjPtr    &query_rslt,
		const AosBitmapObjPtr  &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool
AosIILU64::queryPagingProc(
		const AosQueryContextObjPtr &query_context,
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		bool &has_data,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	// This function is used for query. Its condition is defined by 'query_rslt'.
	// Its mission is to find the starting and ending positions that satisfies
	// the condition defined in 'query_rslt'. It then checks the starting position
	// and block size to make sure:
	// 		[start_idx/start_iilidx, end_idx/end_iilidx]
	// is the range that satisfies the condition and the value range that needs
	// to be copied.
	//	1. get the start iilidx
	//	2. get the end iilidx
	//	3. get the cur iilidx
	//	4. get the block start/end  idx/iilidx
	//
	// Chen Ding, 04/18/2012
	// 'query_bitmap' is not used!!!!!!!!!!!!!!!!!!!!!!!
           
	bool rslt = false;
	
	has_data = true;
	if (query_context->finished())
	{
		OmnAlarm << "Querycontext incorrect" << enderr;
		has_data = false;
		return true;
	}

	bool reverse = query_context->isReverse();;
	AosOpr opr1 = query_context->getOpr();
	AosOpr opr2 = opr1;
	u64 u64_value = query_context->getU64Value();
	u64 u64_value2 = u64_value; 

	if(opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		int rsltInt = AosIILUtil::valueMatch(u64_value, u64_value2);
		aos_assert_r(rsltInt <= 0,false);
		u64_value2 = query_context->getU64Value2();
	}

    if(opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		int rsltInt = AosIILUtil::valueMatch(u64_value, u64_value2);
		aos_assert_r(rsltInt < 0,false);
		u64_value2 = query_context->getU64Value2();
    }

    if(opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		int rsltInt = AosIILUtil::valueMatch(u64_value, u64_value2);
		aos_assert_r(rsltInt < 0,false);
		u64_value2 = query_context->getU64Value2();
    }

    if(opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		int rsltInt = AosIILUtil::valueMatch(u64_value, u64_value2);
		aos_assert_r(rsltInt < 0,false);
		u64_value2 = query_context->getU64Value2();
    }
	
	//1. get the start idx/iilidx
	u64 cur_docid_0 = 0;
	bool isunique = false;
	rslt = nextDocidPrivFind(start_iilidx,
						 u64_value,
						 opr1,
						 cur_docid_0, 
						 isunique,
						 false, // normal order 
						 rdata);

	aos_assert_r(rslt,false);
	
	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		has_data = false;
		return true;
	}
	
	//2. get the end idx/iilidx	
	rslt = nextDocidPrivFind(end_iilidx,
						 u64_value2,
						 opr2,
						 cur_docid_0, 
						 isunique,
						 true, // normal order 
						 rdata);
	aos_assert_r(rslt,false);
	
	rslt = end_iilidx.notFound(mLevel);
	if(rslt)
	{
		aos_assert_r(opr1!=opr2, false);                                                                                       
		has_data = false;
		return true;
	}

	if(query_context->getTotalDocInRslt() == 0)
	{
		query_context->setTotalDocInRslt(countNumRec(start_iilidx, end_iilidx, iilmgrlock, rdata));
	}
	//3. get the cur idx/iilidx, to fix start_idx/iilidx & end_idx/iilidx
	AosIILIdx cur_iilidx = query_context->getIILIndex2();
	
	if (!cur_iilidx.isStart(mLevel))
	{
		u64 cur_value = query_context->getCrtU64Value();
		u64 cur_docid = query_context->getCrtDocid();

		// It moves the 'cursor' to 'cur_iilidx' (from starting).
		nextIndexFixErrorIdx(
					cur_iilidx, 
					cur_value, 
					cur_docid, 
					reverse, 
					has_data, 
					iilmgrlock, 
					rdata);
		if (!has_data)
		{
			query_context->setFinished(true);
			return true;
		}
		
		if (reverse)
		{
			end_iilidx = cur_iilidx;
		}
		else
		{
			start_iilidx = cur_iilidx;
		}
	}
	
	// here is a check, if there are no more docs there, set has_data to false and return.
	if (AosIILIdx::cmpPos(start_iilidx,end_iilidx)> 0)
	{
		// This means 'start_iilidx' > 'end_iilidx', or there
		// are no more data.
		has_data = false;
		return true;
	}

	// If it is grouping query, no need to check the page edge here
	bool isGrouping = query_context->isGrouping();
	if(isGrouping)
	{
		return true;
	}

	//4. get the block start/end  idx/iilidx
	AosIILIdx new_cur_iilidx;
	int bsize = query_context->getBlockSize();
	
	int total = countNumRec(start_iilidx, end_iilidx, iilmgrlock, rdata);
	if(total <= bsize)
	{
		query_context->setFinished(true);		
	}
	else // more than one page
	{	
		query_context->resetIILIndex2();

		// There are more than the needed ones. Need to adjust 
		// [start_iilidx, end_iilidx] so that it contains 'bsize'
		// number of entries. In addition, it will store the 'end position'
		// of this query (so that it may be used for the next block).
		// Note that depending on 'reverse', it may adjust either 
		// 'start_iilidx' or 'end_iilidx'.
		queryPagingProcRec(query_context,
						   start_iilidx,
						   end_iilidx,
						   bsize,
						   reverse,
						   false,
						   iilmgrlock,
						   rdata);
		aos_assert_r(rslt, false);
		if(reverse)
		{
			start_iilidx = query_context->getIILIndex2();
		}
		else
		{
			end_iilidx = query_context->getIILIndex2();
		}
	}

	return true;
}

bool
AosIILU64::copyDataWithCheck(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	
	return false;
}

bool
AosIILU64::copyDataSingle(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLevel == 0, false);
	int start = start_iilidx.getIdx(0);
	int end = start_iilidx.getIdx(0);
	bool reverse = query_context->isReverse();
	if(copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}

	if(copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumDocs-1;
	}

	aos_assert_r(start >= 0 && end <= (int)mNumDocs-1 && start <= end,false);
	
	int size = end - start +1;
	bool isGrouping = query_context->isGrouping();
	if(isGrouping)
	{
		aos_assert_r(query_rslt,false);
		if(reverse)
		{
			for(int i = end;i >= start;i--)
			{
				cur_iilidx.setIdx(0,i);
				query_context->appendEntry(mValues[i],mDocids[i],cur_iilidx,query_rslt);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
		}
		else
		{
			for(int i = start;i <= end;i++)
			{
				cur_iilidx.setIdx(0,i);
				query_context->appendEntry(mValues[i],mDocids[i],cur_iilidx,query_rslt);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
		}
	}
	
	if(query_rslt)
	{
		if(reverse)
		{
			query_rslt->appendBlockRev(&(mDocids[start]),size);
		}
		else
		{
			query_rslt->appendBlock(&(mDocids[start]),size);
		}
	}
	
	if(query_bitmap)
	{
		query_bitmap->appendBlock(&(mDocids[start]),size);
	}
	
	if(query_rslt && query_rslt->isWithValues())
	{
		// copy values
		if(reverse)
		{
			for(int i = end;i >= start;i--)
			{
				query_rslt->appendU64Value(mValues[i]);
			}
		}
		else
		{
			for(int i = start;i <= end;i++)
			{
				query_rslt->appendU64Value(mValues[i]);
			}
		}
	}
	return true;
}

bool
AosIILU64::copyDataRec(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return copyDataSingle(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					copytype,
					iilmgrlock, 
					rdata);
	}
	
	AosIILU64Ptr subiil;
	bool rslt = false;
	int start = start_iilidx.getIdx(mLevel);
	int end = start_iilidx.getIdx(mLevel);
//	bool isGrouping = query_context->isGrouping();
	bool reverse = query_context->isReverse();
	
	if(!reverse)
	{
		if(copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for(int i = 0;i < (int)mNumSubiils;i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,i);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyFromHead)
		{
			aos_assert_r(end >= 0 && end <= (int)mNumSubiils-1,false);
			// copy each subiils
			for(int i = 0;i < end;i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,i);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,end);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyFromHead,
					iilmgrlock, 
					rdata);
			rslt = returnSubIILPriv(end,iilmgrlock, rdata);
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyToTail)
		{
			aos_assert_r(start >= 0 && start <= (int)mNumSubiils-1,false);
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,start);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyToTail,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start,iilmgrlock, rdata);

			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}
			
			for(int i = start+1;i < (int)mNumSubiils;i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				cur_iilidx.setIdx(mLevel,i);
				aos_assert_r(subiil,false);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= (int)mNumSubiils-1 && start <= end,false);
			
			if(start == end)
			{
				subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,start);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyNormal,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(start,iilmgrlock, rdata);
				return rslt;			
			}
			
			
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,start);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyToTail,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start,iilmgrlock, rdata);
			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}

			for(int i = start+1;i < end;i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				cur_iilidx.setIdx(mLevel,i);
				aos_assert_r(subiil,false);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,end);
			rslt = subiil->copyDataRec(query_rslt,
				query_bitmap,
				query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyFromHead,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end,iilmgrlock, rdata);
			return true;
		}
	}
	else// grouping && reverse
	{
		if(copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for(int i = mNumSubiils-1 ;i >= 0 ;i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,i);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyToHead)
		{
			aos_assert_r(end >= 0 && end <= (int)mNumSubiils-1,false);
			// copy each subiils
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,end);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyToHead,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end,iilmgrlock, rdata);
			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}

			for(int i = end-1;i >= 0;i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,i);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyFromTail)
		{
			aos_assert_r(start >= 0 && start <= (int)mNumSubiils-1,false);
			for(int i = (int)mNumSubiils-1; i > start; i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,i);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,start);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyFromTail,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start,iilmgrlock, rdata);
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= (int)mNumSubiils-1 && start <= end,false);
			
			if(start == end)
			{
				subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,start);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyNormal,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(start,iilmgrlock, rdata);
				return rslt;			
			}
			
			
			// copy each subiils
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,end);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyFromTail,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end,iilmgrlock, rdata);
			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}

			for(int i = end -1;i > start;i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil,false);
				cur_iilidx.setIdx(mLevel,i);
				rslt = subiil->copyDataRec(query_rslt,
						query_bitmap,
						query_context,
						start_iilidx,
						end_iilidx,
						cur_iilidx,
						AosIILUtil::eCopyAll,
						iilmgrlock, 
						rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i,iilmgrlock, rdata);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}

			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			cur_iilidx.setIdx(mLevel,start);
			rslt = subiil->copyDataRec(query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					cur_iilidx,
					AosIILUtil::eCopyToHead,
					iilmgrlock, 
					rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start,iilmgrlock, rdata);
			return true;
		}
	}
	return true;
}

bool
AosIILU64::copyData(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	if(query_rslt && query_bitmap)
	{
		return copyDataWithCheck(
					query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					iilmgrlock,
					rdata);
	}
	
	aos_assert_r(query_context,false);
//	bool reverse = query_context->isReverse();
//	if(reverse)
//	{
//		query_rslt->setDataReverse(true);
//	}
	
	if(query_context->isGrouping())
	{
		aos_assert_r(query_rslt,false);
		query_context->appendEntryU64Start(query_rslt);
	}

	AosIILIdx cur_iilidx;
	bool rslt = copyDataRec(query_rslt,
		query_bitmap,
		query_context,
		start_iilidx,
		end_iilidx,
		cur_iilidx,
		AosIILUtil::eCopyNormal, 
		iilmgrlock, 
		rdata);
	if(query_context->isGrouping())
	{
		aos_assert_r(query_rslt,false);
		query_context->appendEntryU64Finish(query_rslt);
	}
	aos_assert_r(rslt, false);
	return true;
}


void
AosIILU64::nextIndexFixErrorIdx(
		AosIILIdx &cur_iilidx,
		const u64 &cur_value,
		const u64 &cur_docid,
		const bool &reverse,
		bool &has_data,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	// This function does the following:
	// 1. Check whether the value [cur_value, cur_docid] at the position
	//    pointed to by 'cur_iilidx' has changed. 
	//    a. If changed, need to find the next one of [cur_value, cur_docid].
	//       i.  If found, return.
	//       ii. If not found, has_data=false.
	//    b. If not changed, continue.
	// 2. Move 'cur_iilidx' to the next one. If 'cur_iilidx' is at the end
	//    of the iil, has_data=false.
	
	bool rslt = false;
// currently we do not fix mismatch
/*	rslt = checkValueDocidRec(cur_iilidx,
					cur_value,
					cur_docid,
					iilmgrlock,
					rdata);
	if(!rslt)
	{
		locateIdx(cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}
	else
*/
	{
		// The entry [cur_value, cur_docid] was not changed
		// (i.e., 'cur_iilidx' points to [cur_value, cur_docid]).
		if(reverse)
		{
			// error handling, if the index is the last one???????????
			rslt = prevQueryPosRec(cur_iilidx,
							iilmgrlock,
							rdata);
			if(!rslt)
			{
				has_data = false;
				return;
			}				
		}
		else
		{
			// error handling, if the index is the last one???????????
			rslt = nextQueryPosRec(cur_iilidx,
							iilmgrlock,
							rdata);
			if(!rslt)
			{
				has_data = false;
				return;				
			}				
		}
	}
	return;
}


bool			
AosIILU64::queryPagingProcRec(
					const AosQueryContextObjPtr &query_context,
					AosIILIdx &start_iilidx,
					AosIILIdx &end_iilidx,
					int &bsize,
					const bool reverse,
					const bool from_start,
					const bool iilmgrlock, 
					const AosRundataPtr &rdata)
{
	// There are more than the needed ones. Need to adjust 
	// [start_iilidx, end_iilidx] so that it contains 'bsize'
	// number of entries. In addition, it will store the 'end position'
	// of this query (so that it may be used for the next block).
	// Note that depending on 'reverse', it may adjust either 
	// 'start_iilidx' or 'end_iilidx'.
	if(isLeafIIL())
	{
		return queryPagingProcSingle(query_context,
					start_iilidx,
					end_iilidx,
					bsize,
					reverse,
					from_start,
					iilmgrlock, 
					rdata);
	}
	//1. proc first subiil
	//2. proc other subiil
	//3. proc last subiil
	bool rslt = false;
	
	AosIILU64Ptr subiil;
	if(reverse)
	{
		int cur_sub_idx = end_iilidx.getIdx(mLevel);
		if(from_start)
		{
			cur_sub_idx = mNumSubiils-1;
		}
		else
		{
			subiil = getSubiilByIndexPriv(cur_sub_idx, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);

			// Handle the first subiil
			rslt = subiil->queryPagingProcRec(query_context,
						start_iilidx,
						end_iilidx,
						bsize,
						reverse,
						false,
						iilmgrlock, 
						rdata);
			aos_assert_r(rslt,false);	
			if(bsize <= 0)
			{
				query_context->setIILIndex2(mLevel,cur_sub_idx);
				return true;
			}
			cur_sub_idx--;
		}
		while(cur_sub_idx >= 0)
		{
			if(mNumEntries[cur_sub_idx] >= bsize)
			{
				break;
			}
			bsize -= mNumEntries[cur_sub_idx];
			cur_sub_idx --;
		}
		aos_assert_r(cur_sub_idx >= 0,false);

		subiil = getSubiilByIndexPriv(cur_sub_idx, iilmgrlock, rdata);
		aos_assert_r(subiil, false);
		rslt = subiil->queryPagingProcRec(query_context,
					start_iilidx,
					end_iilidx,
					bsize,
					reverse,
					true,
					iilmgrlock, 
					rdata);	
		aos_assert_r(rslt,false);	
		aos_assert_r(bsize == 0,false);
		query_context->setIILIndex2(mLevel,cur_sub_idx);
		return true;
	}
	else
	{
		// It is normal order. The current range is:
		// 		[start_iilidx, end_iilidx]
		// It needs 'bsize' number of entries. It recursively
		// asks every subiil (starting from 'start_iilidx')
		// the number of entries.
		int cur_sub_idx = start_iilidx.getIdx(mLevel);
		if(from_start)
		{
			cur_sub_idx = 0;
		}
		else
		{
			// Handle the 'non-regular' case, or the first subiil.
			// If the first subiil contains enough entries, upon 
			// returning, 'bsize' should be 0.
			subiil = getSubiilByIndexPriv(cur_sub_idx, iilmgrlock, rdata);		
			aos_assert_r(subiil,false);
			rslt = subiil->queryPagingProcRec(query_context,
						start_iilidx,
						end_iilidx,
						bsize,
						reverse,
						false,
						iilmgrlock, 
						rdata);	
			aos_assert_r(rslt,false);	
			if(bsize <= 0)
			{
				query_context->setIILIndex2(mLevel,cur_sub_idx);
				return true;
			}
			cur_sub_idx++;
		}

		// When it comes to this point, the first subiil has been
		// handled. The remaining (except the last one) can be
		// calculated.
		while(cur_sub_idx <= (int)mNumSubiils-1)
		{
			if(mNumEntries[cur_sub_idx] >= bsize)
			{
				break;
			}
			bsize -= mNumEntries[cur_sub_idx];
			cur_sub_idx++;
		}
		aos_assert_r(cur_sub_idx < (int)mNumSubiils, false);

		subiil = getSubiilByIndexPriv(cur_sub_idx, iilmgrlock, rdata);
		if (!subiil)
		{
			OmnAlarm << "Failed retrieving subiil" << enderr;
			bsize = 0;
			return false;
		}

		// Handle the last one.
		rslt = subiil->queryPagingProcRec(query_context,
					start_iilidx,
					end_iilidx,
					bsize,
					reverse,
					true,
					iilmgrlock, 
					rdata);	
		aos_assert_r(rslt,false);	
		aos_assert_r(bsize == 0,false);
		query_context->setIILIndex2(mLevel,cur_sub_idx);
		return true;
	}
	
	return true;
}

bool
AosIILU64::queryPagingProcSingle(
		const AosQueryContextObjPtr &query_context,
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		int &bsize,
		const bool reverse,
		const bool from_start,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLevel == 0, false);
	int start = 0;
	int end = mNumDocs - 1;

	if(!from_start)
	{
		if(reverse)
		{
			end = end_iilidx.getIdx(0);
		}
		else
		{
			start = start_iilidx.getIdx(0);
		}
	}
	
	int num_valid = end - start + 1;
	if(bsize > num_valid)
	{
		bsize -= num_valid;
		return true;
	}
	
	// set context
	int cur_pos = reverse ? (end - bsize + 1) : (start + bsize - 1);
	query_context->setIILIndex2(0,cur_pos);
	query_context->setCrtU64Value(mValues[cur_pos]);
	query_context->setCrtDocid(mDocids[cur_pos]);
	bsize = 0;
	return true;
}

bool	
AosIILU64::preQueryNEPriv(
			const AosQueryContextObjPtr &query_context,
			const bool iilmgrlock, 
			const AosRundataPtr &rdata)
{
	return false;
}

bool	
AosIILU64::preQueryPriv(
			const AosQueryContextObjPtr &query_context,
			const bool iilmgrlock, 
			const AosRundataPtr &rdata)
{
	aos_assert_r(query_context,false);
	if(isSingleIIL() && mNumDocs == 0)
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}
	
	AosOpr opr1 = query_context->getOpr();
	if(opr1 == eAosOpr_ne)
	{
		return preQueryNEPriv(query_context,iilmgrlock,rdata);
	}

	//1. get the block start/end idx/iilidx
	AosIILIdx start_iilidx;
	AosIILIdx end_iilidx;
	
	AosOpr opr2 = opr1;
	u64 u64_value = query_context->getU64Value();
	u64 u64_value2 = u64_value; 
	if(opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		u64_value2 = query_context->getU64Value2();
		aos_assert_r(u64_value <= u64_value2, false);
	}

    if(opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		u64_value2 = query_context->getU64Value2();
		aos_assert_r(u64_value < u64_value2, false);
    }

    if(opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		u64_value2 = query_context->getU64Value2();
		aos_assert_r(u64_value < u64_value2, false);
    }

    if(opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		u64_value2 = query_context->getU64Value2();
		aos_assert_r(u64_value < u64_value2, false);
    }

	//1. get the start idx/iilidx
	u64 cur_docid_0 = 0;
	bool isunique = false;

	bool rslt = nextDocidPrivFind(start_iilidx,
						 u64_value,
						 opr1,
						 cur_docid_0, 
						 isunique,
						 false, // normal order 
						 rdata);
	aos_assert_r(rslt,false);
	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		return true;
	}
	
	//2. get the end idx/iilidx	
						 
	rslt = nextDocidPrivFind(end_iilidx,
						 u64_value2,
						 opr2,
						 cur_docid_0, 
						 isunique,
						 true, // normal order 
						 rdata);
	aos_assert_r(rslt,false);
	rslt = end_iilidx.notFound(mLevel);
	if(rslt)
	{
		aos_assert_r(opr1!=opr2, false);
		return true;
	}
	
	if(query_context->getTotalDocInRslt() == 0)
	{
		query_context->setTotalDocInRslt(countNumRec(start_iilidx, end_iilidx, iilmgrlock, rdata));
	}

	return rslt;
}

