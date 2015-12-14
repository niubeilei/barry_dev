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
#include "IILMgr/IILBigStr.h"

#include "API/AosApi.h"
#include "IILMgr/IILMgr.h"
#include "SEInterfaces/BitmapStorageMgrObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"


bool
AosIILBigStr::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->copyFrom(query_rslt);
	bool rslt = queryNewPriv(query_rslt, query_bitmap, query_context, rdata);
	query_context->copyTo(query_rslt);	
	return rslt;
}


bool
AosIILBigStr::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	query_context->setAlphaNum(mIsNumAlpha);

	if (mNumDocs == 0)
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
	//	1. opr == not equal
	//	2. opr == in

	bool hasCmpLen = false;
	AosOpr opr = query_context->getOpr();

	if((int)opr >= 256)
	{
		hasCmpLen = true;
		opr = (AosOpr)((int)opr - 256);
		rdata->setCmpLen(query_context->getStrValue().length());
	}

	if (opr == eAosOpr_ne)
	{
		bool rslt = queryNewNEPriv(query_rslt, query_bitmap, query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}
	else if (opr == eAosOpr_in)
	{
		bool rslt = queryNewInPriv(query_rslt, query_bitmap, query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}

	// Normal query
	//	1. get the block start/end iilidx
	//	2. copy the data
	
	//1. get the block start/end iilidx
	
	AosIILIdx block_start_iilidx;
	AosIILIdx block_end_iilidx;

	bool need_paging = true;
	if (query_rslt && query_bitmap)
	{
		aos_assert_r(!query_bitmap->isEmpty(), false);
		need_paging  = false;
	}
	if (!query_rslt && query_bitmap)
	{
		aos_assert_r(query_bitmap->isEmpty(), false);
		need_paging  = false;
	}
	i64 bsize = query_context->getBlockSize();
	aos_assert_r(bsize >= 0,false);
	if (!bsize || query_context->hasFilter())
	{
		need_paging = false;
	}

	bool has_data = true;
	bool rslt = queryPagingProc(
		query_rslt,	query_bitmap, query_context,
		block_start_iilidx, block_end_iilidx,
		has_data, need_paging, rdata);
	aos_assert_r(rslt, false);

	if (!has_data)
	{
		if (query_rslt) query_rslt->clean();
		if (!query_rslt && query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}

	if (query_context->isMoveOnly() && !query_context->hasFilter())
	{
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}
	//2. copy the data
	if (!query_context->isDetect())
	{
		rslt = copyData(
			query_rslt, query_bitmap, query_context,
			block_start_iilidx, block_end_iilidx, rdata);
		aos_assert_r(rslt, false);
	}
	if (query_context->isMoveOnly())
	{
		if(query_rslt)
		{
			query_rslt->clear();
		}
		if(query_bitmap)
		{
			query_bitmap->clear();
		}
	}
	query_context->setTotalDocInIIL(mNumDocs);
	if(hasCmpLen)
	{
		rdata->setCmpLen(0);
	}
	return true;
}


bool
AosIILBigStr::queryPagingProcDistinct(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		bool &has_data,
		const bool need_paging,
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
	has_data = true;
	
	AosOpr opr1,opr2;
	OmnString str_value, str_value2;
	
	bool reverse = query_context->isReverse();

	//1. get the start idx/iilidx
	OmnString cur_value = query_context->getCrtValue();
	if(cur_value == "")
	{
		opr1 = eAosOpr_an;
	}
	else
	{
		if(reverse)
		{
			opr1 = eAosOpr_lt;
		}
		else
		{
			opr1 = eAosOpr_gt;
		}
		str_value = cur_value; 
	}

	u64 cur_docid_0 = 0;
	bool isunique = true;
	bool rslt = false;
	
	if(reverse)
	{
		rslt = nextDocidPrivFind(end_iilidx, str_value,
			opr1, cur_docid_0, isunique, true, rdata);
		rslt = end_iilidx.notFound(mLevel);
		if (rslt)
		{
			has_data = false;
			return true;
		}
	}
	else
	{
		rslt = nextDocidPrivFind(start_iilidx, str_value,
			opr1, cur_docid_0, isunique, false, rdata);
		rslt = start_iilidx.notFound(mLevel);
		if (rslt)
		{
			has_data = false;
			return true;
		}
	}

	

	//2. get the end idx/iilidx	
	opr2 = eAosOpr_an;
	
	if(reverse)
	{
		rslt = nextDocidPrivFind(start_iilidx, str_value2,
			opr2, cur_docid_0, isunique, false, rdata);
		aos_assert_r(rslt, false);
		
		rslt = start_iilidx.notFound(mLevel);
		if (rslt)
		{
			aos_assert_r(opr1 != opr2, false);
			has_data = false;
			return true;
		}
	}
	else
	{
		rslt = nextDocidPrivFind(end_iilidx, str_value2,
			opr2, cur_docid_0, isunique, true, rdata);
		aos_assert_r(rslt, false);
		
		rslt = end_iilidx.notFound(mLevel);
		if (rslt)
		{
			aos_assert_r(opr1 != opr2, false);
			has_data = false;
			return true;
		}
	}

	// felicia, 2013/03/12
	// to check start_iilidx <= end_iilidx
	if (AosIILIdx::cmpPos(start_iilidx, end_iilidx) > 0)
	{
		has_data = false;
		return true;
	}

	i64 total = query_context->getTotalDocInRslt();
	if (total == 0)
	{
		total = mNumDocs;
		query_context->setTotalDocInRslt(total);
	}

	AosIILIdx orig_start_iilidx = start_iilidx;
	AosIILIdx orig_end_iilidx = end_iilidx;
		
	//3. get the cur idx/iilidx, to fix start_idx/iilidx & end_idx/iilidx
	AosIILIdx cur_iilidx = query_context->getIILIndex2();
	u64 cur_docid = query_context->getCrtDocid();

	//4. get the block start/end  idx/iilidx
	i64 bsize = query_context->getBlockSize();
	aos_assert_r(bsize >= 0,false);
	i64 total_left = countNumRec(start_iilidx, end_iilidx, rdata);
	
	if (total_left <= bsize || bsize == 0)
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
		rslt = queryPagingProcRec(query_context, start_iilidx,
			end_iilidx, bsize, reverse, false, rdata);
		aos_assert_r(rslt, false);
		
		// Ken Lee, 2013/01/16
		aos_assert_r(bsize == 0, false);
		
		if (reverse)
		{
			start_iilidx = query_context->getIILIndex2();
		}
		else
		{
			end_iilidx = query_context->getIILIndex2();
		}
		
		// goto the last entry which equal to crt value
		if(reverse)
		{
			opr2 = eAosOpr_ge;
			str_value2 = query_context->getCrtValue();
			start_iilidx.reset(); 
			rslt = nextDocidPrivFind(start_iilidx, str_value2,
				opr2, cur_docid_0, isunique, false, rdata);
			aos_assert_r(rslt, false);
			
			rslt = start_iilidx.notFound(mLevel);
			if (rslt)
			{
				aos_assert_r(opr1 != opr2, false);
				has_data = false;
				return true;
			}
		}
		else
		{
			opr2 = eAosOpr_le;
			str_value2 = query_context->getCrtValue();
			end_iilidx.reset(); 
			rslt = nextDocidPrivFind(end_iilidx, str_value2,
				opr2, cur_docid_0, isunique, true, rdata);
			aos_assert_r(rslt, false);
			
			rslt = end_iilidx.notFound(mLevel);
			if (rslt)
			{
				aos_assert_r(opr1 != opr2, false);
				has_data = false;
				return true;
			}
		}
		
		// check if it is the last entry
		if(reverse)
		{
			if(isSingleIIL() && str_value2 == mValues[0])
			{
				query_context->setFinished(true);
			}
			if(!isSingleIIL() && str_value2 == mMinVals[0])
			{
				query_context->setFinished(true);
			}
		}
		else
		{
			if(isSingleIIL() && str_value2 == mValues[mNumDocs-1])
			{
				query_context->setFinished(true);
			}
			if(!isSingleIIL() && str_value2 == mMaxVals[mNumSubiils-1])
			{
				query_context->setFinished(true);
			}
		}
	}

	if (reverse)
	{
		u64 query_cursor = countNumRec(
			start_iilidx, orig_end_iilidx, rdata);
		query_context->setQueryCursor(query_cursor);		
	}
	else
	{
		u64 query_cursor = countNumRec(
			orig_start_iilidx, end_iilidx, rdata);
		query_context->setQueryCursor(query_cursor);
	}
	
	return true;
}


bool
AosIILBigStr::queryPagingProc(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		bool &has_data,
		const bool need_paging,
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
	has_data = true;
	if (query_context->finished())
	{
		OmnAlarm << "Querycontext incorrect" << enderr;
		has_data = false;
		return true;
	}

	if (!need_paging)
	{
		query_context->setFinished(true);
	}

	bool reverse = query_context->isReverse();
	AosOpr opr1 = query_context->getOpr();
	OmnString str_value = query_context->getStrValue();
	if((int)opr1 >= 256)
	{
		opr1 = (AosOpr)((int)opr1 - 256);
	}
	AosOpr opr2 = opr1;
	OmnString str_value2 = str_value; 
	u64 cmp_len = rdata->getCmpLen();
	if(opr1 == eAosOpr_distinct)
	{
		return queryPagingProcDistinct(
			query_rslt,
			query_bitmap,
			query_context,
			start_iilidx,
			end_iilidx,
			has_data,
			need_paging,
			rdata);
	}
	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt <= 0, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt <= 0, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt <= 0, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt <= 0, false);
    }
	
	//1. get the start idx/iilidx
	u64 cur_docid_0 = 0;
	bool isunique = true;
	bool rslt = nextDocidPrivFind(start_iilidx, str_value,
		opr1, cur_docid_0, isunique, false, rdata);
	aos_assert_r(rslt, false);

	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		has_data = false;
		return true;
	}
	
	//2. get the end idx/iilidx	
	rslt = nextDocidPrivFind(end_iilidx, str_value2,
		opr2, cur_docid_0, isunique, true, rdata);
	aos_assert_r(rslt, false);
	
	rslt = end_iilidx.notFound(mLevel);
	if (rslt)
	{
		aos_assert_r(opr1 != opr2, false);
		has_data = false;
		return true;
	}

	// felicia, 2013/03/12
	// to check start_iilidx <= end_iilidx
	if (AosIILIdx::cmpPos(start_iilidx, end_iilidx) > 0)
	{
		has_data = false;
		return true;
	}

	i64 total = query_context->getTotalDocInRslt();
	if (total == 0)
	{
		total = countNumRec(start_iilidx, end_iilidx, rdata);
		query_context->setTotalDocInRslt(total);
	}

	// shawn 2014/01/11
	if (query_context->isEstimate())
	{
		getDistrMap(query_context, start_iilidx,
			end_iilidx, reverse, total, rdata);
	}

	AosIILIdx orig_start_iilidx = start_iilidx;
	AosIILIdx orig_end_iilidx = end_iilidx;
		
	//3. get the cur idx/iilidx, to fix start_idx/iilidx & end_idx/iilidx
	AosIILIdx cur_iilidx = query_context->getIILIndex2();
	OmnString cur_value = query_context->getCrtValue();
	u64 cur_docid = query_context->getCrtDocid();
//	u64 page_start = query_context->getPageStart();
//	if (!cur_iilidx.isStart(mLevel))
//	if (page_start > 0 && !query_bitmap)
//	{
//		moveToRec(start_iilidx, end_iilidx, page_start,
//			reverse, has_data, AosIILUtil::eCopyNormal, rdata);
//	}
//	else if (cur_value != "")
	if (cur_value != "")
	{
		// It moves the 'cursor' to 'cur_iilidx' (from starting).
		nextIndexFixErrorIdx(start_iilidx, end_iilidx, cur_iilidx,
			cur_value, cur_docid, reverse, has_data, rdata);
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
	
	if (!has_data)
	{
		return true;
	}

	if (!need_paging)
	{
		if (reverse)
		{
			u64 query_cursor = countNumRec(
				start_iilidx, orig_end_iilidx, rdata);
			query_context->setQueryCursor(query_cursor);
		}
		else
		{
			u64 query_cursor = countNumRec(
				orig_start_iilidx, end_iilidx, rdata);
			query_context->setQueryCursor(query_cursor);
		}
		query_context->setFinished(true);
		return true;
	}

	// If it is grouping query, no need to check the page edge here
	bool isGrouping = query_context->isGrouping();
	if (isGrouping) return true;

	//4. get the block start/end  idx/iilidx
	i64 bsize = query_context->getBlockSize();
	aos_assert_r(bsize >= 0,false);
	i64 total_left = countNumRec(start_iilidx, end_iilidx, rdata);
	if (total_left <= bsize)
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
		rslt = queryPagingProcRec(query_context, start_iilidx,
			end_iilidx, bsize, reverse, false, rdata);
		aos_assert_r(rslt, false);
		
		// Ken Lee, 2013/01/16
		aos_assert_r(bsize == 0, false);
		
		if (reverse)
		{
			start_iilidx = query_context->getIILIndex2();
		}
		else
		{
			end_iilidx = query_context->getIILIndex2();
		}
	}

	if (reverse)
	{
		u64 query_cursor = countNumRec(
			start_iilidx, orig_end_iilidx, rdata);
		query_context->setQueryCursor(query_cursor);		
	}
	else
	{
		u64 query_cursor = countNumRec(
			orig_start_iilidx, end_iilidx, rdata);
		query_context->setQueryCursor(query_cursor);
	}
	
	return true;
}


void
AosIILBigStr::nextIndexFixErrorIdx(
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const OmnString &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
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
	int cmp_len = rdata->getCmpLen();
	rdata->setCmpLen(0);
	rslt = checkValueDocidRec(cur_iilidx, cur_value, cur_docid, rdata);
	rdata->setCmpLen(cmp_len);

	if (!rslt)
	{
		rdata->setCmpLen(0);
		locateIdx(start_iilidx, end_iilidx, cur_iilidx,
			cur_value, cur_docid, reverse, has_data, rdata);
		rdata->setCmpLen(cmp_len);
		return;
	}
	else
	{
		// The entry [cur_value, cur_docid] was not changed
		// (i.e., 'cur_iilidx' points to [cur_value, cur_docid]).
		if (reverse)
		{
			// error handling, if the index is the last one???????????
			rslt = prevQueryPosRec(cur_iilidx, rdata);
			if (!rslt)
			{
				has_data = false;
				return;
			}
		}
		else
		{
			// error handling, if the index is the last one???????????
			rslt = nextQueryPosRec(cur_iilidx, rdata);
			if (!rslt)
			{
				has_data = false;
				return;				
			}				
		}
	}
	return;
}


bool
AosIILBigStr::checkValueDocidRec(
		const AosIILIdx &cur_iilidx,
		const OmnString &cur_value,
		const u64 &cur_docid,
		const AosRundataPtr &rdata)
{
	i64 idx = cur_iilidx.getIdx(mLevel);
	if (mLevel == 0)
	{
		return (idx >= 0 && idx < mNumDocs && cur_value == mValues[idx] && cur_docid == mDocids[idx]);
	}
	
	if (idx < 0 || idx >= mNumSubiils)
	{
		return false;
	}

	AosIILBigStrPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);

	return subiil->checkValueDocidRec(
		cur_iilidx, cur_value, cur_docid, rdata);
}


bool			
AosIILBigStr::queryPagingProcRec(
		const AosQueryContextObjPtr &query_context,
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		i64 &bsize,
		const bool reverse,
		const bool from_start,
		const AosRundataPtr &rdata)
{
	// There are more than the needed ones. Need to adjust 
	// [start_iilidx, end_iilidx] so that it contains 'bsize'
	// number of entries. In addition, it will store the 'end position'
	// of this query (so that it may be used for the next block).
	// Note that depending on 'reverse', it may adjust either 
	// 'start_iilidx' or 'end_iilidx'.
	if (isLeafIIL())
	{
		return queryPagingProcSingle(query_context, start_iilidx,
			end_iilidx, bsize, reverse, from_start, rdata);
	}
	//1. proc first subiil
	//2. proc other subiil
	//3. proc last subiil
	bool rslt = false;
	AosIILBigStrPtr subiil;
	if (reverse)
	{
		i64 cur_sub_idx = end_iilidx.getIdx(mLevel);
		if (from_start)
		{
			cur_sub_idx = mNumSubiils - 1;
		}
		else
		{
			subiil = getSubiilByIndexPriv(cur_sub_idx, rdata);
			aos_assert_r(subiil, false);

			// Handle the first subiil
			rslt = subiil->queryPagingProcRec(query_context, start_iilidx,
				end_iilidx, bsize, reverse, false, rdata);
			aos_assert_r(rslt, false);
			
			// mod by shawn 13-02-25  if (bsize <= 0)
			if (bsize <= 0 || cur_sub_idx == 0)
			{
				query_context->setIILIndex2(mLevel, cur_sub_idx);
				return true;
			}
			if (cur_sub_idx == 0)
			{
				query_context->setIILIndex2(mLevel, cur_sub_idx);
				return true;
			}
			cur_sub_idx--;
		}
		//ken 2013/01/02
		//while (cur_sub_idx >= 0)
		while (cur_sub_idx > 0)
		{
			if (mNumEntries[cur_sub_idx] >= bsize)
			{
				break;
			}
			bsize -= mNumEntries[cur_sub_idx];
			cur_sub_idx--;
		}
		aos_assert_r(cur_sub_idx >= 0, false);

		subiil = getSubiilByIndexPriv(cur_sub_idx, rdata);
		aos_assert_r(subiil, false);

		rslt = subiil->queryPagingProcRec(
			query_context, start_iilidx, end_iilidx,
			bsize, reverse, true, rdata);	
		aos_assert_r(rslt, false);	
		
		// Ken Lee, 2013/01/16
		//aos_assert_r(bsize == 0, false);
		query_context->setIILIndex2(mLevel, cur_sub_idx);
		return true;
	}
	else
	{
		// It is normal order. The current range is:
		// 		[start_iilidx, end_iilidx]
		// It needs 'bsize' number of entries. It recursively
		// asks every subiil (starting from 'start_iilidx')
		// the number of entries.
		i64 cur_sub_idx = start_iilidx.getIdx(mLevel);
		if (from_start)
		{
			cur_sub_idx = 0;
		}
		else
		{
			// Handle the 'non-regular' case, or the first subiil.
			// If the first subiil contains enough entries, upon 
			// returning, 'bsize' should be 0.
			subiil = getSubiilByIndexPriv(cur_sub_idx, rdata);
			aos_assert_r(subiil, false);

			rslt = subiil->queryPagingProcRec(query_context, start_iilidx,
				end_iilidx, bsize, reverse, false, rdata);	
			aos_assert_r(rslt, false);
			// mod by shawn 13-02-25  if (bsize <= 0)
			if (bsize <= 0 || cur_sub_idx == mNumSubiils-1)
			{
				query_context->setIILIndex2(mLevel, cur_sub_idx);
				return true;
			}
			cur_sub_idx++;
		}

		// When it comes to this point, the first subiil has been
		// handled. The remaining (except the last one) can be
		// calculated.
		
		//ken 2013/01/02
		//while (cur_sub_idx <= mNumSubiils-1)
		while (cur_sub_idx < mNumSubiils-1)
		{
			if (mNumEntries[cur_sub_idx] >= bsize)
			{
				break;
			}
			bsize -= mNumEntries[cur_sub_idx];
			cur_sub_idx++;
		}
		aos_assert_r(cur_sub_idx < mNumSubiils, false);

		subiil = getSubiilByIndexPriv(cur_sub_idx, rdata);
		aos_assert_r(subiil, false);

		// Handle the last one.
		rslt = subiil->queryPagingProcRec(query_context, start_iilidx,
			end_iilidx, bsize, reverse, true, rdata);	
		aos_assert_r(rslt, false);	
		
		// Ken Lee, 2013/01/16
		//aos_assert_r(bsize == 0, false);
		query_context->setIILIndex2(mLevel, cur_sub_idx);
		return true;
	}
	return true;
}


bool
AosIILBigStr::queryPagingProcSingle(
		const AosQueryContextObjPtr &query_context,
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		i64 &bsize,
		const bool reverse,
		const bool from_start,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLevel == 0, false);
	i64 start = 0;
	i64 end = mNumDocs - 1;
	if (!from_start)
	{
		if (reverse)
		{
			end = end_iilidx.getIdx(0);
		}
		else
		{
			start = start_iilidx.getIdx(0);
		}
	}
	
	i64 num_valid = end - start + 1;
	if (bsize > num_valid)
	{
		bsize -= num_valid;
		return true;
	}
	
	// set context
	i64 cur_pos = reverse ? (end - bsize + 1) : (start + bsize - 1);
	query_context->setIILIndex2(0, cur_pos);
	query_context->setCrtValue(mValues[cur_pos]);
	query_context->setCrtDocid(mDocids[cur_pos]);
	bsize = 0;
	return true;
}


bool
AosIILBigStr::copyData(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const AosRundataPtr &rdata)
{
	bool reverse = query_context->isReverse();
	if (reverse && query_rslt)
	{
		query_rslt->setDataReverse(true);
	}
	
	if (query_context->isGrouping())
	{
		aos_assert_r(query_rslt, false);
		query_context->appendGroupEntryStrStart(query_rslt);
	}

	AosIILIdx cur_iilidx;
	bool rslt = copyDataRec(
		query_rslt, query_bitmap, query_context,
		start_iilidx, end_iilidx, cur_iilidx,
		AosIILUtil::eCopyNormal, rdata);
	aos_assert_r(rslt, false);

	if (query_context->isGrouping())
	{
		aos_assert_r(query_rslt, false);
		query_context->appendGroupEntryStrFinish(query_rslt);
	}

	bool withCheck = (query_rslt && query_bitmap);
	bool hasFilter = query_context->hasFilter();
	if (withCheck || hasFilter)
	{
		int rsltInt = 0;
		query_context->setIILIndex2(cur_iilidx);
		OmnString cur_value;
		u64 cur_docid = 0;
		bool rslt = getValueByIdxRecPriv(cur_iilidx, cur_value, cur_docid, rdata);
		aos_assert_r(rslt,false);
		query_context->setCrtValue(cur_value);
		query_context->setCrtDocid(cur_docid);

		if (reverse)
		{
			rsltInt = AosIILIdx::cmpPos(cur_iilidx, start_iilidx);
		}
		else
		{
			rsltInt = AosIILIdx::cmpPos(end_iilidx, cur_iilidx);
		}
		aos_assert_r(rsltInt >= 0, false);

		if (rsltInt == 0)
		{
			query_context->setFinished(true);
		}
		else
		{
			query_context->setFinished(false);
		}
	}
	return true;
}


bool
AosIILBigStr::copyDataRec(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return copyDataSingle(query_rslt, query_bitmap, query_context,
			start_iilidx, end_iilidx, cur_iilidx, copytype, rdata);
	}
	
	bool rslt = false;
	i64 num_docs = 0;
	AosIILBigStrPtr subiil;
	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);
//	bool isGrouping = query_context->isGrouping();
	bool reverse = query_context->isReverse();
	if (reverse)
	{
		if (copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for (i64 i = mNumSubiils-1; i >= 0; i--)
			{
				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyToHead)
		{
			aos_assert_r(end >= 0 && end <= mNumSubiils-1, false);

			// copy each subiils
			subiil = getSubiilByIndexPriv(end, rdata);		
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToHead, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(end, rdata);
			aos_assert_r(rslt, false);

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				return true;
			}

			for (i64 i = end-1; i >= 0; i--)
			{
				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyFromTail)
		{
			aos_assert_r(start >= 0 && start <= mNumSubiils-1, false);

			for (i64 i = mNumSubiils-1; i > start; i--)
			{
				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, rdata);
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromTail, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(start, rdata);
			aos_assert_r(rslt, false);

			return true;
		}
		
		if (copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= mNumSubiils - 1 && start <= end, false);
			
			if (start == end)
			{
				subiil = getSubiilByIndexPriv(start, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx, 
					AosIILUtil::eCopyNormal, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(start, rdata);
				aos_assert_r(rslt, false);

				return true;
			}
			
			// copy each subiils
			subiil = getSubiilByIndexPriv(end, rdata);		
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToHead, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(end, rdata);
			aos_assert_r(rslt, false);

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				return true;
			}
			
			for (i64 i = end - 1; i > start; i--)
			{
				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}

			subiil = getSubiilByIndexPriv(start, rdata);
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromTail, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(start, rdata);
			aos_assert_r(rslt, false);

			return true;
		}
	}
	else // normal
	{
		if (copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for (i64 i = 0; i < mNumSubiils; i++)
			{
				subiil = getSubiilByIndexPriv(i, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyFromHead)
		{
			aos_assert_r(end >= 0 && end <= mNumSubiils-1, false);

			// copy each subiils
			for (i64 i = 0; i < end; i++)
			{
				subiil = getSubiilByIndexPriv(i, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}

			subiil = getSubiilByIndexPriv(end, rdata);		
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromHead, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(end, rdata);
			aos_assert_r(rslt, false);

			return true;
		}
		
		if (copytype == AosIILUtil::eCopyToTail)
		{
			aos_assert_r(start >= 0 && start <= mNumSubiils-1, false);

			// copy each subiils
			subiil = getSubiilByIndexPriv(start, rdata);
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToTail, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(start, rdata);
			aos_assert_r(rslt, false);

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				return true;
			}

			for (i64 i=start+1; i<mNumSubiils; i++)
			{
				subiil = getSubiilByIndexPriv(i, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= mNumSubiils-1 && start <= end, false);
			
			if (start == end)
			{
				subiil = getSubiilByIndexPriv(start, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyNormal, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(start, rdata);
				aos_assert_r(rslt, false);

				return rslt;
			}
			
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, rdata);
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToTail, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(start, rdata);
			aos_assert_r(rslt, false);

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				return true;
			}

			for (i64 i = start + 1; i < end; i++)
			{
				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}

			subiil = getSubiilByIndexPriv(end, rdata);		
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromHead, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(end, rdata);
			aos_assert_r(rslt, false);

			return true;
		}
	}

	return true;
}


bool
AosIILBigStr::copyDataSingle(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLevel == 0, false);

	i64 start = start_iilidx.getIdx(0);
	i64 end = end_iilidx.getIdx(0);
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}

	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumDocs - 1;
	}

	aos_assert_r(start >= 0 && end <= mNumDocs-1 && start <= end, false);

	i64 size = end - start + 1;
	aos_assert_r(size >= 0, false);

	if (size == 0) return true;

	i64 num_docs = 0;
	bool reverse = query_context->isReverse();
	bool onlyBitmap = (!query_rslt && query_bitmap);
	bool withCheck = (query_rslt && query_bitmap);
	bool hasFilter = query_context->hasFilter();

	bool isGrouping = query_context->isGrouping();
	if (isGrouping)
	{
		aos_assert_r(query_rslt, false);

		if (reverse)
		{
			for (i64 i=end; i>=start; i--)
			{
				cur_iilidx.setIdx(0, i);

				if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
				{
					continue;
				}

				query_context->appendGroupEntry(mValues[i],
					mDocids[i], cur_iilidx, query_rslt, mIsNumAlpha);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
		}
		else
		{
			for (i64 i=start; i<=end; i++)
			{
				cur_iilidx.setIdx(0, i);

				if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
				{
					continue;
				}

				query_context->appendGroupEntry(mValues[i],
					mDocids[i], cur_iilidx, query_rslt, mIsNumAlpha);

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
		}
		return true;
	}

	aos_assert_r(query_rslt || query_bitmap, false);

	if (withCheck || hasFilter)
	{
		if (AosIILMgr::smCopyDataWithThrd && size >= AosIILMgr::smCopyDataWithThrdIILSize)
		{
			return copyDataSingleWithThrd(query_rslt, query_bitmap,
				query_context, start, end, cur_iilidx, rdata);
		}

		if (reverse)
		{
			for (i64 i=end; i>=start; i--)
			{
				cur_iilidx.setIdx(0, i);

				if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
				{
					continue;
				}

				if (hasFilter && !query_context->evalFilter(
					mValues[i], mDocids[i], rdata))
				{
					continue;	
				}

				if (onlyBitmap)
				{
					query_bitmap->appendDocid(mDocids[i]);
				}
				else
				{
					query_rslt->appendDocid(mDocids[i]);
					if (query_rslt->isWithValues())
					{
						query_rslt->appendStrValue(mValues[i]);
					}
				}

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
		}
		else
		{
			for (i64 i=start; i<=end; i++)
			{
				cur_iilidx.setIdx(0, i);
				
				if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
				{
					continue;
				}

				if (hasFilter && !query_context->evalFilter(
					mValues[i], mDocids[i], rdata))
				{
					continue;	
				}
				
				if (onlyBitmap)
				{
					query_bitmap->appendDocid(mDocids[i]);
				}
				else
				{
					query_rslt->appendDocid(mDocids[i]);
					if (query_rslt->isWithValues())
					{
						query_rslt->appendStrValue(mValues[i]);
					}
				}

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
		}
		return true;
	}

	if (query_bitmap)
	{
		query_bitmap->appendDocids(&(mDocids[start]), size);
	}

	if (query_rslt)
	{
		bool addValue = query_rslt->isWithValues();
		if (reverse)
		{
			query_rslt->appendBlockRev(&(mDocids[start]), size);
			if (addValue)
			{
				for (i64 i = end; i >= start; i--)
				{
					query_rslt->appendStrValue(mValues[i]);
				}
			}
		}
		else
		{
			query_rslt->appendBlock(&(mDocids[start]), size);
			if (addValue)
			{
				for (i64 i = start; i <= end; i++)
				{
					query_rslt->appendStrValue(mValues[i]);
				}
			}
		}
	}

	return true;
}


bool
AosIILBigStr::getSplitValuePriv(
		const i64 &num_blocks,
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(num_blocks > 1, false);

	i64 num_splitters = num_blocks-1;
	i64 cur_split_num = 0;
	i64 num_each_split = mNumDocs/num_blocks;
	i64 cur_doc_num = num_each_split;
	OmnString cur_value;
	return getSplitValueRecPriv(
		num_splitters, cur_split_num, num_each_split,
		cur_doc_num, cur_value, query_rslt, rdata);
}


bool
AosIILBigStr::getSplitValueRecPriv(
		i64 &num_splitters, 
		i64 &cur_split_num, 
		i64 &num_each_split, 
		i64 &cur_doc_num,
		OmnString &cur_value,
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return 	getSplitValueSinglePriv(
			num_splitters, cur_split_num, num_each_split,
			cur_doc_num, cur_value, query_rslt, rdata);
	}
	
	AosIILBigStrPtr subiil;
	bool rslt = false;
	for (i64 i = 0; i < mNumSubiils; i++)
	{
		if (mNumEntries[i] < cur_doc_num)
		{
			cur_doc_num -= mNumEntries[i];
		}
		else
		{
			subiil = getSubiilByIndexPriv(i, rdata);
			aos_assert_r(subiil, false);

			rslt = subiil->getSplitValueRecSafe(
				num_splitters, cur_split_num, num_each_split,
				cur_doc_num, cur_value, query_rslt, rdata);
			aos_assert_r(rslt, false);

			if (cur_split_num >= num_splitters)
			{
				return true;
			}
		}
	}
	return true;
}


bool
AosIILBigStr::getSplitValueSinglePriv(
		i64 &num_splitters, 
		i64 &cur_split_num, 
		i64 &num_each_split, 
		i64 &cur_doc_num,
		OmnString &cur_value,
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	if (mNumDocs < cur_doc_num)
	{
		cur_doc_num -= mNumDocs;
		return true;
	}
	i64 cur_pos = 0;
	u64 cmp_len = 0;
	rdata->setCmpLen(0);

	while (cur_doc_num + cur_pos <= mNumDocs)
	{
		//1. get the cur_pos
		cur_pos += (cur_doc_num-1);
		cur_doc_num = 0;
		aos_assert_r(cur_pos < mNumDocs, false);

		if (AosIILUtil::valueNMatch(mValues[cur_pos],cur_value,mIsNumAlpha,cmp_len)==0)
		{
			if (AosIILUtil::valueNMatch(mValues[mNumDocs-1],cur_value,mIsNumAlpha,cmp_len)==0)
			{
				return true;
			}
			while (cur_pos < mNumDocs && AosIILUtil::valueNMatch(mValues[cur_pos],cur_value,mIsNumAlpha,cmp_len)==0)
			{
				cur_pos ++;
			}
			aos_assert_r(cur_pos < mNumDocs, false);
		}
		// append value,reset doc_num
		query_rslt->appendDocid(mDocids[cur_pos]);
		query_rslt->appendStrValue(mValues[cur_pos]);
		cur_value = mValues[cur_pos];
		cur_split_num ++;
		if (cur_split_num >= num_splitters)
		{
			return true;
		}
		cur_doc_num = num_each_split;		
	}
	
	
	return true;
}


bool
AosIILBigStr::getSplitValuePriv(
		const AosQueryContextObjPtr &query_context,
		const i64 &size,
		vector<AosQueryContextObjPtr> &contexts,
		const AosRundataPtr &rdata)
{
	aos_assert_r(size > 0, false);

	AosOpr opr = query_context->getOpr();
	OmnString str_value = query_context->getStrValue();

	if((int)opr >= 256)
	{
		opr = (AosOpr)((int)opr - 256);
		rdata->setCmpLen(str_value.length());
	}

	if ((opr == eAosOpr_ne) || (opr == eAosOpr_like))	
	{
		OmnNotImplementedYet;
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return false;
	}
	
	AosOpr opr1 = opr;
	AosOpr opr2 = opr;
	OmnString str_value2 = str_value; 
	u64 cmp_len = rdata->getCmpLen();
	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt <= 0, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt < 0, false);
    }

	AosIILIdx start_iilidx;
	AosIILIdx end_iilidx;
	
	// find start iilidx
	u64 cur_docid_0 = 0;
	bool isunique = true;
	bool rslt = nextDocidPrivFind(
		start_iilidx, str_value, opr1,
		cur_docid_0, isunique, false, rdata);
	aos_assert_r(rslt, false);

	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}
	// find end iilidx
	rslt = nextDocidPrivFind(
		end_iilidx, str_value2, opr2,
		cur_docid_0, isunique, true, rdata);
	aos_assert_r(rslt, false);

	rslt = end_iilidx.notFound(mLevel);
	if (rslt)
	{
		aos_assert_r(opr1 != opr2, false);
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}

	i64 total = countNumRec(start_iilidx, end_iilidx, rdata);
	if (query_context->getTotalDocInRslt() == 0)
	{
		query_context->setTotalDocInRslt(total);
	}

	i64 min_for_each = query_context->getPageSize();
	if (total <= min_for_each)
	{
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}


	i64 each = total / size;
	int size2 = size;
	if (each < min_for_each)
	{
		size2 = (total / min_for_each) + 1;
		each = total / size2;
	}
	if(each == 0)
	{
		size2 = total;
		each = 1;
	}
	vector<OmnString> splitKeys;
	vector<OmnString> values;
	vector<OmnString>::iterator itr;
	i64 num_left = each;
	i64 saperator_left = size2 - 1;
	rslt = getSplitValueRec(
		AosIILUtil::eCopyNormal, start_iilidx, end_iilidx,
		each, num_left, saperator_left, splitKeys, rdata);
	aos_assert_r(rslt, false);
	
	AosQueryContextObj::SplitValueType svtype = query_context->getSplitValueType();
	OmnString svstr = query_context->getSplitValueStr();
	AosConvertAsciiBinary(svstr);

	OmnString vv;
	itr = splitKeys.begin();
	while (itr != splitKeys.end())
	{
		vv = getSVStr(*itr, svtype, svstr);
		if (vv != "") values.push_back(vv);
		itr++;
	}

	// remove no use saperator
	itr = values.begin(); 
	OmnString last_value;
	while (itr != values.end()) 
	{
		if (compWithSVType(*itr,str_value,svtype) || compWithSVType(*itr,str_value2,svtype) ||
				(itr != values.begin() && compWithSVType(*itr, last_value,svtype)))
		{
			values.erase(itr);
			continue;
		}
		last_value = *itr;
		itr++;
	}

	if (values.size() == 0)
	{
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}
		
	// proc the first one
	// clone query_context to query_contexts
	AosQueryContextObjPtr cur_context = query_context->clone();
	
	if (opr1 == eAosOpr_ge)
	{
		cur_context->setOpr(eAosOpr_range_ge_lt);
		cur_context->setStrValue(str_value);
		cur_context->setStrValue2(values[0]);
	}
	else if (opr1 == eAosOpr_gt)
	{
		cur_context->setOpr(eAosOpr_range_ge_lt);		
		cur_context->setStrValue(str_value);
		cur_context->setStrValue2(values[0]);
	}
	else
	{
		cur_context->setOpr(eAosOpr_lt);		
		cur_context->setStrValue(values[0]);
	}
	contexts.push_back(cur_context);

	// proc the mid ones
	for (u32 i = 1; i < values.size(); i++)
	{
		cur_context = query_context->clone();
		cur_context->setOpr(eAosOpr_range_ge_lt);
		cur_context->setStrValue(values[i-1]);
		cur_context->setStrValue2(values[i]);
		// 
		contexts.push_back(cur_context);
	}
	
	// proc the last one
	cur_context = query_context->clone();
	
	if (opr1 == eAosOpr_lt)
	{
		cur_context->setOpr(eAosOpr_range_ge_lt);
		cur_context->setStrValue(values[values.size()-1]);
		cur_context->setStrValue2(str_value2);
	}
	else if (opr1 == eAosOpr_le)
	{
		cur_context->setOpr(eAosOpr_range_ge_le);		
		cur_context->setStrValue(values[values.size()-1]);
		cur_context->setStrValue2(str_value2);
	}
	else
	{
		cur_context->setOpr(eAosOpr_ge);
		cur_context->setStrValue(values[values.size()-1]);
	}
	contexts.push_back(cur_context);
	return true;
}


bool
AosIILBigStr::getSplitValueRec(
		const AosIILUtil::CopyType type,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const i64 &each,
		i64 &num_left,
		i64 &saperator_num_left,
		vector<OmnString> &values,
		const AosRundataPtr &rdata)
{
	AosIILBigStrPtr subiil;
	if (isLeafIIL())
	{
		return getSplitValueSingle(
			type, start_iilidx, end_iilidx, each, num_left,
			saperator_num_left, values, rdata);
	}
	
	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);

	switch(type)
	{
	case AosIILUtil::eCopyAll :
		 {
			for (i64 i = 0; i < mNumSubiils; i++)
			{
				if (num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
					
				subiil = getSubiilByIndexPriv(i, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, rdata);
				if (saperator_num_left <= 0) return true;
			}
		 }			
		 break;

	case AosIILUtil::eCopyFromHead :
		 {
			for (i64 i = 0; i < end - 1; i++)
			{
				if (num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
				
				subiil = getSubiilByIndexPriv(i, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, rdata);
				if (saperator_num_left <= 0) return true;
			}
			
			//proc the last one
			subiil = getSubiilByIndexPriv(end, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyFromHead, start_iilidx, end_iilidx, each,
				num_left, saperator_num_left, values, rdata);
			if (saperator_num_left <= 0) return true;
		 }
		 break;

	case AosIILUtil::eCopyToTail :
		 {
			subiil = getSubiilByIndexPriv(start, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyToTail, start_iilidx, end_iilidx, each,
				num_left, saperator_num_left, values, rdata);
			if (saperator_num_left <= 0) return true;
			for (i64 i = start + 1; i < mNumSubiils; i++)
			{
				if (num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
					
				subiil = getSubiilByIndexPriv(i, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, rdata);
				if (saperator_num_left <= 0) return true;
			}
		 }
		 break;

	case AosIILUtil::eCopyNormal :
		 {
			if (start == end)
			{
				subiil = getSubiilByIndexPriv(start, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyNormal, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, rdata);
				return true;
			}
			
			// start != end
			subiil = getSubiilByIndexPriv(start, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyToTail, start_iilidx, end_iilidx, each,
				num_left, saperator_num_left, values, rdata);
			if (saperator_num_left <= 0) return true;

			for (i64 i = start + 1; i < end - 1; i++)
			{
				if (num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
					
				subiil = getSubiilByIndexPriv(i, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, rdata);
				if (saperator_num_left <= 0) return true;
			}
			subiil = getSubiilByIndexPriv(end, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyFromHead, start_iilidx, end_iilidx, each, 
				num_left, saperator_num_left, values, rdata);
			if (saperator_num_left <= 0) return true;
		 } 
		 break;

	default:
		// not right
		OmnAlarm << "Type not right : "<< (int)type << enderr;
		return false;
	}
	return true;
}


bool	
AosIILBigStr::getSplitValueSingle(
		const AosIILUtil::CopyType type,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const i64 &each,
		i64 &num_left,
		i64 &saperator_num_left,
		vector<OmnString> &values,
		const AosRundataPtr &rdata)
{
	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);

	switch(type)
	{
		case AosIILUtil::eCopyAll:
			start = 0;
			end = mNumDocs -1;
			break;
		case AosIILUtil::eCopyFromHead: 
			start = 0;
			break;
		case AosIILUtil::eCopyToTail:
			end = mNumDocs -1;
			break;
		case AosIILUtil::eCopyNormal:
			break;
		default:		
			break;
	}    
	
	i64 cur = start;
	while (end-cur+1 >= num_left)
	{
		// 1. shift num_left
		// 2. record value
		// 3. saperator_num_left --
		// 4. reset num_left
		cur += num_left;
		values.push_back(mValues[cur-1]);
		saperator_num_left --;
		if (saperator_num_left<= 0)return true;
		num_left = each;
	}
	
	// not enough for a piece
	num_left -= (end-cur+1);
	return true;
}         
          

bool	
AosIILBigStr::preQueryNEPriv(
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}


bool
AosIILBigStr::preQueryPriv(
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	if (isSingleIIL() && mNumDocs == 0)
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}
	
	AosOpr opr1 = query_context->getOpr();
	OmnString str_value = query_context->getStrValue();

	bool hasCmpLen = false;
	rdata->setCmpLen(0);	
	if((int)opr1 >= 256)
	{
		hasCmpLen = true;
		opr1 = (AosOpr)((int)opr1 - 256);
		rdata->setCmpLen(str_value.length());
	}

	if (opr1 == eAosOpr_ne)
	{
		bool rslt = preQueryNEPriv(query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}

	//1. get the block start/end idx/iilidx
	AosIILIdx start_iilidx;
	AosIILIdx end_iilidx;

	AosOpr opr2 = opr1;
	OmnString str_value2 = str_value; 
	u64 cmp_len = rdata->getCmpLen();
	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt <= 0, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueNMatch(str_value, str_value2, mIsNumAlpha,cmp_len);
		aos_assert_r(rsltInt < 0, false);
    }

	//1. get the start idx/iilidx
	u64 cur_docid_0 = 0;
	bool isunique = true;

	bool rslt = nextDocidPrivFind(
		start_iilidx, str_value, opr1,
		cur_docid_0, isunique, false, rdata);
	aos_assert_r(rslt, false);

	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}
	
	//2. get the end idx/iilidx	
						 
	rslt = nextDocidPrivFind(
		end_iilidx, str_value2, opr2,
		cur_docid_0, isunique, true, rdata);
	aos_assert_r(rslt, false);

	rslt = end_iilidx.notFound(mLevel);
	if (rslt)
	{
		aos_assert_r(opr1 != opr2, false);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}

	i64 total = countNumRec(start_iilidx, end_iilidx, rdata);
	if (query_context->getTotalDocInRslt() == 0)
	{
		query_context->setTotalDocInRslt(total);
	}

	if(hasCmpLen)
	{
		rdata->setCmpLen(0);
	}
	return rslt;
}


bool
AosIILBigStr::compWithSVType(
		const OmnString &str1,
		const OmnString &str2,
		AosQueryContextObj::SplitValueType svtype)
{
	switch(svtype)
	{
		case AosQueryContextObj::eKey:
		case AosQueryContextObj::eKeyValue:
		case AosQueryContextObj::eFirstField:
		case AosQueryContextObj::eFirstSecondField:
			 return str1 == str2;

		default:
			return false;
	}
	return false;
}


OmnString
AosIILBigStr::getSVStr(
		const OmnString &key,
		AosQueryContextObj::SplitValueType svtype,
		const OmnString &sep)
{
	i64 idx = -1;
	i64 idx2 = -1;
	OmnString rslt = "";
	switch (svtype)
	{
	case AosQueryContextObj::eKey:
	case AosQueryContextObj::eKeyValue:
		 rslt = key;
		 break;
		
	case AosQueryContextObj::eFirstField:
		 idx = key.indexOf(sep, 0);
		 if (idx < 0)
		 {
			rslt = key;
		 }
		 else if (idx > 0)
		 {
			rslt.assign(&(key.data()[0]), idx); 
		 }
		 break;

	case AosQueryContextObj::eFirstSecondField:
		 idx = key.indexOf(sep, 0);
		 if (idx < 0)
		 {
			rslt = key;
			break;
		 }
		 else if (idx == 0)
		 {
		 	break;
		 }

		 idx2 = key.indexOf(sep, idx + sep.length());
		 if (idx2 <= 0)
		 {
			rslt = key;
		 }
		 else
		 {
		 	rslt.assign(&(key.data()[0]), idx2);
		 }
		 
		 break;

	default:
		break;
	}

	return rslt;
}


bool
AosIILBigStr::bitmapQueryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	query_context->setAlphaNum(mIsNumAlpha);
	
	if (mNumDocs == 0)
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
	//	1. opr == like
	//	2. opr == not equal
	AosOpr opr = query_context->getOpr();

	bool hasCmpLen = false;
	rdata->setCmpLen(0);
	if((int)opr >= 256)
	{
		hasCmpLen = true;
		opr = (AosOpr)((int)opr - 256);
		rdata->setCmpLen(query_context->getStrValue().length());
	}
	
	if (opr == eAosOpr_ne)
	{
		bool rslt = bitmapQueryNewNEPriv(query_rslt, query_bitmap, query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}
	else if (opr == eAosOpr_in)
	{
		bool rslt = bitmapQueryNewInPriv(query_rslt, query_bitmap, query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}

	// Normal query
	//	1. get the block start/end iilidx
	//	2. copy the data
	
	//1. get the block start/end iilidx
	
	AosIILIdx block_start_iilidx;
	AosIILIdx block_end_iilidx;
	
	bool need_paging = true;
	i64 bsize = query_context->getBlockSize();
	aos_assert_r(bsize >= 0,false);
	if (!bsize || query_context->hasFilter())
	{
		need_paging = false;
	}
	
	bool has_data = true;
	bool rslt = queryPagingProc(
		query_rslt,	query_bitmap, query_context, 
		block_start_iilidx, block_end_iilidx, 
		has_data, need_paging, rdata);
	aos_assert_r(rslt, false);

	if (!has_data)
	{
		if (query_rslt) query_rslt->clean();
		if (!query_rslt && query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}

	OmnString limit_value = query_context->getLimitStrValue();
	u64 limit_docid = query_context->getLimitDocid();
	if (limit_docid)
	{
		setLimitation(block_start_iilidx, block_end_iilidx, limit_value,
			limit_docid, query_context->isReverse(), has_data, rdata);
	}

	if (!has_data)
	{
		// if the data has been limited, the finish flag should NOT be set to true
		if (query_rslt) query_rslt->clean();
		if (!query_rslt && query_bitmap) query_bitmap->clean();
		query_context->setTotalDocInIIL(mNumDocs);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}

	//2. copy the data
	if (query_context->hasFilter())
	{
		rslt = copyData(0, query_bitmap, query_context, 
			block_start_iilidx, block_end_iilidx, rdata);	
		query_context->setNumInRslt(query_bitmap->getNumBits());
	}
	else
	{
		u64 num_in_rslt = countNumRec(block_start_iilidx,block_end_iilidx, rdata);
		query_context->setNumInRslt(num_in_rslt);
		
		rslt = copyBitmap(query_rslt, query_bitmap, query_context, 
			block_start_iilidx, block_end_iilidx, rdata);	
	}
	query_context->setTotalDocInIIL(mNumDocs);
	if(hasCmpLen)
	{
		rdata->setCmpLen(0);
	}
	return rslt;
}


bool
AosIILBigStr::copyBitmap(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const AosRundataPtr &rdata)
{
	AosIILIdx cur_iilidx;
	bool rslt = copyBitmapRec(
		query_rslt, query_bitmap, query_context,
		start_iilidx, end_iilidx, cur_iilidx,
		AosIILUtil::eCopyNormal, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigStr::copyBitmapRec(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return copyBitmapSingle(
			query_rslt, query_bitmap, query_context, start_iilidx,
			end_iilidx, cur_iilidx, copytype, rdata);
	}
	
	AosIILBigStrPtr subiil;
	bool rslt = false;
	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);
//	bool reverse = query_context->isReverse();
	if (copytype == AosIILUtil::eCopyFromHead)
	{
		aos_assert_r(end >= 0 && end <= mNumSubiils - 1, false);

		// copy each subiils
		if (end > 0)
		{
			vector<u64> remote_list;
			vector<u64> local_parent_list;
			vector<u64> local_list;
			AosBitmapTreeObjPtr tree = getBitmapTree(rdata);
			aos_assert_r(tree, false);

			tree->queryNodes(rdata,mIILIds[0],mIILIds[end-1],remote_list,local_parent_list,local_list);
			for (u32 i = 0; i < remote_list.size(); i++)
			{
				query_rslt->appendDocid(remote_list[i]);
			}
			AosBitmapObjPtr local_bitmap = 0;
			for (u32 i = 0; i < local_list.size(); i++)
			{
				local_bitmap = AosGetBitmapStorageMgr()->retrieveLeafAsBitmap(rdata,local_list[i],local_parent_list[i]);
				aos_assert_r(local_bitmap, false);

				query_bitmap->orBitmap(local_bitmap);
			}
		}

		subiil = getSubiilByIndexPriv(end, rdata);		
		aos_assert_r(subiil, false);

		cur_iilidx.setIdx(mLevel, end);
		rslt = subiil->copyBitmapRec(
			query_rslt, query_bitmap, query_context,
			start_iilidx, end_iilidx, cur_iilidx,
			AosIILUtil::eCopyFromHead, rdata);
		aos_assert_r(rslt, false);

		rslt = returnSubIILPriv(end, rdata);
		aos_assert_r(rslt, false);
		return true;
	}
	
	if (copytype == AosIILUtil::eCopyToTail)
	{
		aos_assert_r(start >= 0 && start <= mNumSubiils - 1, false);

		// copy each subiils
		subiil = getSubiilByIndexPriv(start, rdata);
		aos_assert_r(subiil, false);

		cur_iilidx.setIdx(mLevel, start);
		rslt = subiil->copyBitmapRec(
			query_rslt, query_bitmap, query_context,
			start_iilidx, end_iilidx, cur_iilidx,
			AosIILUtil::eCopyToTail, rdata);
		aos_assert_r(rslt, false);

		rslt = returnSubIILPriv(start, rdata);
		aos_assert_r(rslt, false);

		i64 num_docs = getRsltNumDocs(query_rslt, query_bitmap);
		aos_assert_r(num_docs >= 0, false);

		if (query_context->isFull(num_docs))
		{
			return true;
		}

		if (start < mNumSubiils-1)
		{
			vector<u64> remote_list;
			vector<u64> local_parent_list;
			vector<u64> local_list;

			AosBitmapTreeObjPtr tree = getBitmapTree(rdata);
			aos_assert_r(tree, false);

			tree->queryNodes(rdata,mIILIds[start+1],mIILIds[mNumSubiils-1],remote_list,local_parent_list,local_list);
			for (u32 i = 0; i < remote_list.size(); i++)
			{
				query_rslt->appendDocid(remote_list[i]);
			}
			AosBitmapObjPtr local_bitmap = 0;
			for (u32 i = 0; i < local_list.size(); i++)
			{
				local_bitmap = AosGetBitmapStorageMgr()->retrieveLeafAsBitmap(rdata,local_list[i],local_parent_list[i]);
				aos_assert_r(local_bitmap, false);

				query_bitmap->orBitmap(local_bitmap);
			}
		}
		return true;
	}
		
	if (copytype == AosIILUtil::eCopyNormal)
	{
		aos_assert_r(start >= 0 && end <= mNumSubiils-1 && start <= end, false);
		
		if (start == end)
		{
			subiil = getSubiilByIndexPriv(start, rdata);
			aos_assert_r(subiil, false);

			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyBitmapRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyNormal, rdata);
			aos_assert_r(rslt, false);

			rslt = returnSubIILPriv(start, rdata);
			aos_assert_r(rslt, false);
			return true;
		}
		
		// copy each subiils
		subiil = getSubiilByIndexPriv(start,  rdata);
		aos_assert_r(subiil, false);

		cur_iilidx.setIdx(mLevel, start);
		rslt = subiil->copyBitmapRec(
			query_rslt, query_bitmap, query_context,
			start_iilidx, end_iilidx, cur_iilidx,
			AosIILUtil::eCopyToTail, rdata);
		aos_assert_r(rslt, false);

		rslt = returnSubIILPriv(start, rdata);
		aos_assert_r(rslt, false);

		if (start < end-1)
		{
			vector<u64> remote_list;
			vector<u64> local_parent_list;
			vector<u64> local_list;

			AosBitmapTreeObjPtr tree = getBitmapTree(rdata);
			aos_assert_r(tree, false);

			tree->queryNodes(rdata,mIILIds[start+1],mIILIds[end-1],remote_list,local_parent_list,local_list);
			for (u32 i = 0; i < remote_list.size(); i++)
			{
				query_rslt->appendDocid(remote_list[i]);
			}
			AosBitmapObjPtr local_bitmap = 0;
			for (u32 i = 0; i < local_list.size(); i++)
			{
				local_bitmap = AosGetBitmapStorageMgr()->retrieveLeafAsBitmap(rdata,local_list[i],local_parent_list[i]);
				aos_assert_r(local_bitmap, false);

				query_bitmap->orBitmap(local_bitmap);
			}
		}

		subiil = getSubiilByIndexPriv(end, rdata);		
		aos_assert_r(subiil, false);

		cur_iilidx.setIdx(mLevel, end);
		rslt = subiil->copyBitmapRec(
			query_rslt, query_bitmap, query_context,
			start_iilidx, end_iilidx, cur_iilidx,
			AosIILUtil::eCopyFromHead, rdata);
		aos_assert_r(rslt, false);

		rslt = returnSubIILPriv(end, rdata);
		aos_assert_r(rslt, false);
		return true;
	}
	return true;
}


bool
AosIILBigStr::copyBitmapSingle(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLevel == 0, false);

//	bool reverse = query_context->isReverse();
	i64 start = start_iilidx.getIdx(0);
	i64 end = end_iilidx.getIdx(0);
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}

	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumDocs - 1;
	}

	aos_assert_r(start >= 0 && end <= mNumDocs - 1 && start <= end, false);
	
	i64 size = end - start + 1;
	query_bitmap->appendDocids(&(mDocids[start]), size);
	return true;
}



bool
AosIILBigStr::bitmapRsltQueryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt && query_bitmap, false);

	if (mNumDocs == 0)
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
	//	1. opr == like
	//	2. opr == not equal
	AosOpr opr = query_context->getOpr();

	bool hasCmpLen = false;

	rdata->setCmpLen(0);
	if((int)opr >= 256)
	{
		hasCmpLen = true;
		opr = (AosOpr)((int)opr - 256);
		rdata->setCmpLen(query_context->getStrValue().length());
	}

	if (opr == eAosOpr_ne)
	{
		bool rslt = bitmapRsltQueryNewNEPriv(query_rslt, query_bitmap, query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}
	else if (opr == eAosOpr_in)
	{
		bool rslt = bitmapRsltQueryNewInPriv(query_rslt, query_bitmap, query_context, rdata);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return rslt;
	}

	// Normal query
	//	1. get the block start/end iilidx
	//	2. copy the data
	
	//1. get the block start/end iilidx
	
	AosIILIdx block_start_iilidx;
	AosIILIdx block_end_iilidx;

	bool need_paging = true;
	i64 bsize = query_context->getBlockSize();
	aos_assert_r(bsize >= 0,false);
	if (!bsize || query_context->hasFilter())
	{
		need_paging = false;
	}

	bool has_data = true;
	bool rslt = queryPagingProc(
		query_rslt,	query_bitmap, query_context, 
		block_start_iilidx, block_end_iilidx,
		has_data, need_paging, rdata);
	aos_assert_r(rslt, false);

	if (!has_data)
	{
//		if (query_rslt) query_rslt->clean();
		query_context->setFinished(true);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}

	OmnString limit_value = query_context->getLimitStrValue();
	u64 limit_docid = query_context->getLimitDocid();
	if (limit_docid)
	{
		setLimitation(block_start_iilidx, block_end_iilidx, limit_value,
			limit_docid, query_context->isReverse(), has_data, rdata);
	}

	if (!has_data)
	{
		// if the data has been limited, the finish flag should NOT be set to true
		query_context->setTotalDocInIIL(mNumDocs);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
		return true;
	}
	//2. copy the data
	rslt = copyDataWithBitmap(query_rslt, query_bitmap, 
		query_context, block_start_iilidx, block_end_iilidx, rdata);	
	query_context->setTotalDocInIIL(mNumDocs);
		if(hasCmpLen)
		{
			rdata->setCmpLen(0);
		}
	return rslt;
}


bool
AosIILBigStr::copyDataWithBitmap(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const AosRundataPtr &rdata)
{
	bool reverse = query_context->isReverse();
	if (reverse && query_rslt)
	{
		query_rslt->setDataReverse(true);
	}
	
	if (query_context->isGrouping())
	{
		aos_assert_r(query_rslt, false);
		query_context->appendGroupEntryStrStart(query_rslt);
	}

	AosIILIdx cur_iilidx;
	bool rslt = copyDataWithBitmapRec(
		query_rslt, query_bitmap, query_context,
		start_iilidx, end_iilidx, cur_iilidx,
		AosIILUtil::eCopyNormal, rdata);
	if (query_context->isGrouping())
	{
		aos_assert_r(query_rslt, false);
		query_context->appendGroupEntryStrFinish(query_rslt);
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigStr::copyDataWithBitmapRec(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const AosIILUtil::CopyType copytype, 
		const AosRundataPtr &rdata)
{
	// if all data copied, return true;
	if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
	{
		return true;
	}

	bool parent_ignored = false;
	u64 crt_parent_id = 0;
	vector<u64> child_list;

	if (isLeafIIL())
	{
		return copyDataSingle(query_rslt, query_bitmap, query_context,
			start_iilidx, end_iilidx, cur_iilidx, copytype, rdata);
	}
	
	AosIILBigStrPtr subiil;
	bool rslt = false;
	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);
//	bool isGrouping = query_context->isGrouping();
	bool reverse = query_context->isReverse();
	if (reverse)
	{
		if (copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for (i64 i = mNumSubiils-1 ; i >= 0 ; i--)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyToHead)
		{
			aos_assert_r(end >= 0 && end <= mNumSubiils-1, false);
			// copy each subiils
			if (checkWithBitmap(end, query_bitmap, parent_ignored,
				crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(end, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, end);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyToHead, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(end, rdata);
				aos_assert_r(rslt, false);
			}
			
			if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
			{
				return true;
			}

			for (i64 i = end - 1; i >= 0; i--)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyFromTail)
		{
			aos_assert_r(start >= 0 && start <= mNumSubiils-1, false);
			for (i64 i = mNumSubiils-1; i > start; i--)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}

			// copy each subiils
			if (checkWithBitmap(start, query_bitmap, parent_ignored,
				crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(start, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyFromTail, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(start, rdata);
				aos_assert_r(rslt, false);
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= mNumSubiils-1 && start <= end, false);
			
			if (start == end)
			{
				if (checkWithBitmap(start, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					subiil = getSubiilByIndexPriv(start, rdata);
					aos_assert_r(subiil, false);

					cur_iilidx.setIdx(mLevel, start);
					rslt = subiil->copyDataWithBitmapRec(
						query_rslt, query_bitmap, query_context,
						start_iilidx, end_iilidx, cur_iilidx, 
						AosIILUtil::eCopyNormal, rdata);
					aos_assert_r(rslt, false);

					rslt = returnSubIILPriv(start, rdata);
					aos_assert_r(rslt, false);
				}
				return true;
			}
			
			// copy each subiils
			if (checkWithBitmap(end, query_bitmap, parent_ignored, crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(end, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, end);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyToHead, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(end, rdata);
				aos_assert_r(rslt, false);
			}
			
			if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
			{
				return true;
			}
			
			for (i64 i = end - 1; i > start; i--)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}

			if (checkWithBitmap(start, query_bitmap, parent_ignored,
				crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(start, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyFromTail, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(start, rdata);
				aos_assert_r(rslt, false);
			}
			return true;
		}
	}
	else 
	{
		if (copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for (i64 i = 0; i < mNumSubiils; i++)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
						crt_parent_id, child_list,rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyFromHead)
		{
			aos_assert_r(end >= 0 && end <= mNumSubiils - 1, false);
			// copy each subiils
			for (i64 i = 0; i < end; i++)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list,rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}
			
			if (checkWithBitmap(end, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(end, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, end);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyFromHead, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(end, rdata);
				aos_assert_r(rslt, false);
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyToTail)
		{
			aos_assert_r(start >= 0 && start <= mNumSubiils - 1, false);
			// copy each subiils
			if (checkWithBitmap(start, query_bitmap, parent_ignored,
				crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(start, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyToTail, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(start, rdata);
				aos_assert_r(rslt, false);
			}

			if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
			{
				return true;
			}

			for (i64 i = start + 1; i < mNumSubiils; i++)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}
			return true;
		}
		
		if (copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= mNumSubiils-1 && start <= end, false);
			
			if (start == end)
			{
				if (checkWithBitmap(start, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					subiil = getSubiilByIndexPriv(start, rdata);
					aos_assert_r(subiil, false);

					cur_iilidx.setIdx(mLevel, start);
					rslt = subiil->copyDataWithBitmapRec(
						query_rslt, query_bitmap, query_context,
						start_iilidx, end_iilidx, cur_iilidx,
						AosIILUtil::eCopyNormal, rdata);
					aos_assert_r(rslt, false);

					rslt = returnSubIILPriv(start, rdata);
					aos_assert_r(rslt, false);
				}
				return true;
			}
			
			// copy each subiils
			if (checkWithBitmap(start, query_bitmap, parent_ignored,
				crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(start, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyToTail, rdata);
				aos_assert_r(rslt, false);
				
				rslt = returnSubIILPriv(start, rdata);
				aos_assert_r(rslt, false);
			}
			
			if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
			{
				return true;
			}

			for (i64 i = start + 1; i < end; i++)
			{
				if (!checkWithBitmap(i, query_bitmap, parent_ignored,
					crt_parent_id, child_list, rdata))
				{
					continue;
				}

				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(i, rdata);
				aos_assert_r(rslt, false);

				if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
				{
					return true;
				}
			}

			if (checkWithBitmap(end, query_bitmap, parent_ignored,
				crt_parent_id, child_list, rdata))
			{
				subiil = getSubiilByIndexPriv(end, rdata);		
				aos_assert_r(subiil, false);

				cur_iilidx.setIdx(mLevel, end);
				rslt = subiil->copyDataWithBitmapRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyFromHead, rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILPriv(end, rdata);
				aos_assert_r(rslt, false);
			}
			return true;
		}
	}
	return true;
}


bool
AosIILBigStr::checkWithBitmap(
		const int &pos,
		const AosBitmapObjPtr &query_bitmap,
		bool &parent_ignored,
		u64 &crt_parent_id,
		vector<u64> &child_list,
		const AosRundataPtr &rdata)
{
	if (mLevel != 1) return true;
	// if the id is in the list
	vector<u64>::iterator result = find(child_list.begin( ), child_list.end( ), mIILIds[pos]);
	if (result == child_list.end())// not found
	{
		AosBitmapTreeObjPtr tree = getBitmapTree(rdata);
		aos_assert_r(tree, false);

		crt_parent_id = tree->getFamilyNodeID(mIILIds[pos],child_list);
		// check parent
		AosBitmapObjPtr bitmap = AosGetBitmapStorageMgr()->retrieveLeafAsBitmap(rdata,crt_parent_id,crt_parent_id);
		aos_assert_r(bitmap, false);

		parent_ignored = ! AosBitmapObj::getObject()->checkAndBitmap(bitmap,query_bitmap);

		if (parent_ignored)
		{
			return false;
		}
		// parent can't be ignored
		parent_ignored = false;
		
		// check child
		bitmap = AosGetBitmapStorageMgr()->retrieveLeafAsBitmap(rdata,mIILIds[pos],crt_parent_id);
		aos_assert_r(bitmap, false);

		bool leaf_ignored = !AosBitmapObj::getObject()->checkAndBitmap(bitmap,query_bitmap);
		
		return !leaf_ignored;
	}
	else
	{
		if (parent_ignored) return false;

		AosBitmapObjPtr bitmap = AosGetBitmapStorageMgr()->retrieveLeafAsBitmap(rdata,mIILIds[pos],crt_parent_id);
		aos_assert_r(bitmap, false);

		return AosBitmapObj::getObject()->checkAndBitmap(bitmap,query_bitmap);
	}
	return true;
}


bool
AosIILBigStr::locateIdx(
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		const OmnString &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	AosOpr opr;
	if (reverse)
	{
		opr = eAosOpr_lt;
	}
	else
	{
		opr = eAosOpr_gt;
	}

	bool rslt = findPosPriv(opr, cur_value,
		cur_docid, cur_iilidx, reverse, rdata);
	if (!rslt)
	{
		has_data = false;
		OmnAlarm << "Error happened in finding position." << enderr;
		return rslt;
	}
	
	bool valid = true;
	
	if (!cur_iilidx.isValid(0))
	{
		valid = false;
	}
		
	if (AosIILIdx::cmpPos(start_iilidx, cur_iilidx) > 0)
	{
		if (reverse)
		{
			valid = false;
		}
		else
		{
			cur_iilidx = start_iilidx;
		}
	}
		
	if (AosIILIdx::cmpPos(end_iilidx, cur_iilidx) < 0 )
	{
		if (reverse)
		{
			cur_iilidx = end_iilidx;
		}
		else
		{
			valid = false;
		}
	}
	if (!valid)
	{
		cur_iilidx.setInvalid();
		has_data = false;
	}
	return true;
}


bool	
AosIILBigStr::setLimitation(
		AosIILIdx &start_iilidx,
		AosIILIdx &end_iilidx,
		const OmnString &limit_value,
		const u64 &limit_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	has_data = true;
	AosIILIdx limit_iilidx;
	AosOpr opr;
	if (reverse)
	{
		opr = eAosOpr_gt;
	}
	else
	{
		opr = eAosOpr_lt;
	}

	bool rslt = findPosPriv(opr, limit_value,
		limit_docid, limit_iilidx, reverse, rdata);
	if (!rslt)
	{
		has_data = false;
		OmnAlarm << "Error happened in finding position." << enderr;
		return rslt;
	}
	
	if (!limit_iilidx.isValid(0))
	{
		has_data = false;
		return true;
	}
	
	if (reverse)
	{
		if (AosIILIdx::cmpPos(end_iilidx, limit_iilidx) < 0)
		{
			has_data = false;
			return true;
		}			

		if (AosIILIdx::cmpPos(start_iilidx, limit_iilidx) < 0)
		{
			start_iilidx = limit_iilidx;
		}
		return true;
	}
	else 
	{
		if (AosIILIdx::cmpPos(start_iilidx, limit_iilidx) > 0)
		{
			has_data = false;
			return true;
		}			

		if (AosIILIdx::cmpPos(end_iilidx, limit_iilidx) > 0)
		{
			end_iilidx = limit_iilidx;
		}
		return true;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


bool	
AosIILBigStr::getDistrMap(
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const bool reverse,
		const i64 &total,
		const AosRundataPtr &rdata)
{
	aos_assert_r(total > 0, false);

	AosIILIdx cur_iilidx = start_iilidx;
	if (reverse)
	{
		cur_iilidx = end_iilidx;
	}
	
	u64 step = total / 300;
	if (step == 0)step = 1;

	vector<OmnString> values;
	vector<u64>	docids;
	vector<u64>	num_entries;
	getDistrMapRec(query_context, start_iilidx, end_iilidx, cur_iilidx,
		values, docids, num_entries, reverse, step, rdata);
	int size = values.size();

	vector<OmnString> values_rslt;
	vector<u64>	docids_rslt;
	vector<u64>	num_entries_rslt;
	if (size > 300)
	{
		int cur_pos = 0;
		int cur_count = 0;

		if (reverse)
		{
			cur_pos = size-1;
			values_rslt.push_back( values[cur_pos]);
			docids_rslt.push_back( docids[cur_pos]);

			for (;cur_pos >= 0 ;cur_pos--)
			{
				if (cur_count >= total/300)
				{
					//1. set value/docids
					values_rslt.push_back( values[cur_pos]);
					docids_rslt.push_back( docids[cur_pos]);
					num_entries_rslt.push_back(cur_count);
					cur_count = 0;
				}
				cur_count += num_entries[cur_pos];
			}
			num_entries_rslt.push_back(cur_pos);
			values_rslt.swap(values);
			docids_rslt.swap(docids);
			num_entries.swap(num_entries_rslt);
		}
		else // normal
		{
			values_rslt.push_back( values[cur_pos]);
			docids_rslt.push_back( docids[cur_pos]);

			for (;cur_pos < size;cur_pos++)
			{
				if (cur_count >= total/300)
				{
					//1. set value/docids
					values_rslt.push_back( values[cur_pos]);
					docids_rslt.push_back( docids[cur_pos]);
					num_entries_rslt.push_back(cur_count);
					cur_count = 0;
				}
				cur_count += num_entries[cur_pos];
			}
			num_entries_rslt.push_back(cur_pos);
			
			values_rslt.swap(values);
			docids_rslt.swap(docids);
			num_entries.swap(num_entries_rslt);
		}
	}
	
	if (reverse)
	{
		vector<OmnString> values2;
		vector<u64>	docids2;
		vector<u64>	num_entries2;
		u32 total = values.size();
		for (u32 i = 0; i < total; i++)
		{
			values2.push_back(values[total-1-i]);
			docids2.push_back(docids[total-1-i]);
			num_entries2.push_back(num_entries[total-1-i]);
		}
		query_context->setDistrMap(values2,docids2,num_entries2);
	}
	else
	{
		query_context->setDistrMap(values,docids,num_entries);
	}
	
	aos_assert_r(values.size() == docids.size(), false);
	aos_assert_r(num_entries.size() == docids.size(), false);
	if (0)
	{
		OmnScreen << "============Distr Map============" << endl;
		for (u32 i = 0; i < values.size(); i++)
		{
			OmnScreen << docids[i] << "\n" << values[i] << "\n" << num_entries[i] << endl;
		}
		OmnScreen << "=================================" << endl;
	}
	return true;
}


bool	
AosIILBigStr::getDistrMapRec(
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &cur_iilidx,
		vector<OmnString> &values, 
		vector<u64> &docids, 
		vector<u64> &num_entries,
		const bool reverse,
		const u64 &step,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mLevel == 0)
	{
		int cur_idx;
		bool is_start = (AosIILIdx::cmpPos(start_iilidx,cur_iilidx,mLevel+1) == 0);
		bool is_end = (AosIILIdx::cmpPos(end_iilidx,cur_iilidx,mLevel+1) == 0);
		int start = 0;
		int end = mNumDocs-1;
		if (is_start)start = start_iilidx.getIdx(mLevel);
		if (is_end)end = end_iilidx.getIdx(mLevel);

		cur_idx = start;
		if (reverse)
		{
			while (cur_idx <= end)
			{
				if (end - cur_idx +1 <= (int)step)
				{
					
					values.push_back(mValues[end]);
					docids.push_back(mDocids[end]);
					num_entries.push_back(end - cur_idx +1);
					break;
				}
				values.push_back(mValues[cur_idx]);
				docids.push_back(mDocids[cur_idx]);
				num_entries.push_back(step);
				cur_idx += step;
			}
		}
		else
		{
			while (cur_idx <= end)
			{
				values.push_back(mValues[cur_idx]);
				docids.push_back(mDocids[cur_idx]);
				if (end - cur_idx +1 <= (int)step)
				{
					num_entries.push_back(end - cur_idx +1);
					break;
				}
				num_entries.push_back(step);
				cur_idx += step;
			}
		}
		return true;
	}
	
	// level 1 or higher
	bool is_start = (AosIILIdx::cmpPos(start_iilidx,cur_iilidx,mLevel+1) == 0);
	bool is_end = (AosIILIdx::cmpPos(end_iilidx,cur_iilidx,mLevel+1) == 0);
	int start = 0;
	int end = mNumSubiils-1;
	if (is_start)start = start_iilidx.getIdx(mLevel);
	if (is_end)end = end_iilidx.getIdx(mLevel);
	AosIILBigStrPtr subiil;

	if (mLevel == 1 || mNumSubiils >= 300)
	{
		int s_idx = 0;
		int e_idx = 0;
		u64 entries = 0;

		for (int i = start; i <= end; i++)
		{
			cur_iilidx.setIdx(mLevel,i);
			bool is_start2 = (AosIILIdx::cmpPos(start_iilidx,cur_iilidx,mLevel) == 0);
			bool is_end2 = (AosIILIdx::cmpPos(end_iilidx,cur_iilidx,mLevel) == 0);
			
			s_idx = 0;
			e_idx = mNumEntries[i]-1;
			if (is_start2)s_idx = start_iilidx.getIdx(0);
			if (is_end2)e_idx = end_iilidx.getIdx(0);
			entries = e_idx - s_idx +1;
			
			if (entries <= step)
			{
				if (reverse)
				{
					values.push_back(mMaxVals[i]);
					docids.push_back(mMaxDocids[i]);
					num_entries.push_back(entries);
				}
				else
				{
					values.push_back(mMinVals[i]);
					docids.push_back(mMinDocids[i]);
					num_entries.push_back(entries);
				}
			}
			else // entries > step
			{
				subiil = getSubiilByIndexPriv(i, rdata);
				aos_assert_r(subiil, false);

				rslt = subiil->getDistrMapRec(
					query_context, start_iilidx, end_iilidx, cur_iilidx,
					values, docids, num_entries, reverse, step, rdata);
				aos_assert_r(rslt, false);
			}			
		}
		return true;
	}
	
	// mLevel == 2 or higher
	for (int i = start; i < end; i++)
	{
		cur_iilidx.setIdx(mLevel,i);

		subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);

		rslt = subiil->getDistrMapRec(
			query_context, start_iilidx, end_iilidx, cur_iilidx,
			values, docids, num_entries, reverse, step, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosIILBigStr::queryNewNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	bool rslt = true;
	bool reverse = query_context->isReverse();
	AosQueryContextObjPtr query_context2 = query_context->clone();
	u64 orig_cursor = query_context2->getQueryCursor();
	u64 block_size_left = query_context2->getBlockSize();
	aos_assert_r(block_size_left >= 0,false);
	AosQueryRsltObjPtr query_rslt2;
	if (query_rslt)
	{
		query_rslt2 = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt2->setWithValues(query_rslt->isWithValues());
	}
	AosBitmapObjPtr query_bitmap2;
	if (query_bitmap) query_bitmap2 = query_bitmap->clone(rdata);

	if (reverse)
	{
		query_context2->setOpr(eAosOpr_gt);
		rslt = queryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if (query_context2->finished())
		{
			i64 moved = query_context2->getQueryCursor() - orig_cursor;
			aos_assert_r(moved >= 0,false);
			if(block_size_left > 0)
			{
				block_size_left -= moved;
				if(block_size_left == 0)
				{
					query_context->setBackward(query_context2);
					query_context->setFinished(false);
					query_context->setCrtValue(query_context->getStrValue());
					query_context->setCrtDocid(1);
					return true;
				}
			}

			query_context2 = query_context->clone();
			query_context2->setOpr(eAosOpr_lt);
			query_context2->setBlockSize(block_size_left);
			rslt = queryNewPriv(query_rslt2, query_bitmap2, query_context2, rdata);
			aos_assert_r(rslt, false);

			query_context->setFinished(query_context2->finished());
			// merge the result
			if (query_rslt)
			{
				query_rslt->setDataReverse(reverse);
				query_rslt->mergeFrom(query_rslt2);
			}
			else
			{
				aos_assert_r(query_bitmap, false);
				query_bitmap->orBitmap(query_bitmap2);
			}
		}
	}
	else
	{
		query_context2->setOpr(eAosOpr_lt);
		rslt = queryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if (query_context2->finished())
		{
			i64 moved = query_context2->getQueryCursor() - orig_cursor;
			aos_assert_r(moved >= 0,false);
			if(block_size_left > 0)
			{
				block_size_left -= moved;
				if(block_size_left == 0)
				{
					query_context->setBackward(query_context2);
					query_context->setFinished(false);
					query_context->setCrtValue(query_context->getStrValue());
					query_context->setCrtDocid(1);
					return true;
				}
			}
			query_context2 = query_context->clone();
			query_context2->setOpr(eAosOpr_gt);
			query_context2->setBlockSize(block_size_left);
			
			rslt = queryNewPriv(query_rslt2, query_bitmap2, query_context2, rdata);
			aos_assert_r(rslt, false);
			
			// merge the result
			query_context->setFinished(query_context2->finished());
			if (query_rslt)
			{
				query_rslt->setDataReverse(reverse);
				query_rslt->mergeFrom(query_rslt2);
			}
			else
			{
				aos_assert_r(query_bitmap, false);
				query_bitmap->orBitmap(query_bitmap2);
			}
		}
	}
	
	query_context->setBackward(query_context2);
	return true;
}


bool
AosIILBigStr::bitmapQueryNewNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt && query_bitmap, false);

	bool rslt = true;
	bool reverse = query_context->isReverse();
	AosQueryContextObjPtr query_context2 = query_context->clone();
	u64 orig_cursor = query_context2->getQueryCursor();
	u64 block_size_left = query_context2->getBlockSize();
	aos_assert_r(block_size_left >= 0, false);
	AosQueryRsltObjPtr query_rslt2 = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt2->setWithValues(query_rslt->isWithValues());
	AosBitmapObjPtr query_bitmap2 = query_bitmap->clone(rdata);

	if (reverse)
	{
		query_context2->setOpr(eAosOpr_gt);
		rslt = bitmapQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if(query_context2->finished())
		{
			i64 moved = query_context2->getQueryCursor() - orig_cursor;
			aos_assert_r(moved >= 0,false);
			if(block_size_left > 0)
			{
				block_size_left -= moved;
				if(block_size_left == 0)
				{
					query_context->setBackward(query_context2);
					query_context->setFinished(false);
					query_context->setCrtValue(query_context->getStrValue());
					query_context->setCrtDocid(1);
					return true;
				}
			}
			query_context2 = query_context->clone();
			query_context2->setOpr(eAosOpr_lt);
			query_context2->setBlockSize(block_size_left);
			rslt = bitmapQueryNewPriv(query_rslt2, query_bitmap2, query_context2, rdata);
			aos_assert_r(rslt, false);

			query_context->setFinished(query_context2->finished());
			// merge the result
			query_rslt->setDataReverse(reverse);
			query_rslt->mergeFrom(query_rslt2);
			query_bitmap->orBitmap(query_bitmap2);
		}
	}
	else
	{
		query_context2->setOpr(eAosOpr_lt);
		rslt = bitmapQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if (query_context2->finished())
		{
			i64 moved = query_context2->getQueryCursor() - orig_cursor;
			if(block_size_left > 0)
			{
				block_size_left -= moved;
				if(block_size_left == 0)
				{
					query_context->setBackward(query_context2);
					query_context->setFinished(false);
					query_context->setCrtValue(query_context->getStrValue());
					query_context->setCrtDocid(1);
					return true;
				}
			}
			query_context2 = query_context->clone();
			query_context2->setOpr(eAosOpr_gt);
			query_context2->setBlockSize(block_size_left);

			
			rslt = bitmapQueryNewPriv(query_rslt2, query_bitmap2, query_context2, rdata);
			aos_assert_r(rslt, false);
			
			query_context->setFinished(query_context2->finished());
			// merge the result
			query_rslt->setDataReverse(reverse);
			query_rslt->mergeFrom(query_rslt2);
			query_bitmap->orBitmap(query_bitmap2);
		}
	}
	
	query_context->setBackward(query_context2);
	return true;
}


bool
AosIILBigStr::bitmapRsltQueryNewNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt && query_bitmap, false);

	bool rslt = true;
	bool reverse = query_context->isReverse();
	AosQueryContextObjPtr query_context2 = query_context->clone();
	u64 block_size = query_context->getBlockSize();

	if (reverse)
	{
		query_context2->setOpr(eAosOpr_gt);
		rslt = bitmapRsltQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if(block_size == 0 || query_rslt->getNumDocs() < (i64)block_size)
		{
			query_context2 = query_context->clone();
			query_context2->setOpr(eAosOpr_lt);

			rslt = bitmapRsltQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		query_context2->setOpr(eAosOpr_lt);
		rslt = bitmapRsltQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if(block_size == 0 || query_rslt->getNumDocs() < (i64)block_size)
		{
			query_context2 = query_context->clone();
			query_context2->setOpr(eAosOpr_gt);
			
			rslt = bitmapRsltQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
			aos_assert_r(rslt, false);
		}
	}
	
	return true;
}


bool
AosIILBigStr::queryNewInPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	AosQueryContextObjPtr query_context2 = query_context->clone();
	AosQueryRsltObjPtr query_rslt2;
	AosBitmapObjPtr query_bitmap2;
	AosBitmapObjPtr orig_bitmap;
	if (query_bitmap) orig_bitmap = query_bitmap->clone(rdata);

	OmnString str_value = query_context->getStrValue();
	vector<OmnString> cond_strs;
	AosSplitStr(str_value, ";", cond_strs, eMaxNumOprIn);
	aos_assert_r(cond_strs.size() > 0 && cond_strs.size() < eMaxNumOprIn, false);

	vector<InCond> conds;
	if(query_context->isReverse())
	{
		for (i64 i = cond_strs.size()-1; i >= 0; i--)
		{
			if (cond_strs[i] != "")
			{
				conds.push_back(InCond(cond_strs[i]));
			}
		}
	}
	else
	{
		for (u64 i = 0; i < cond_strs.size(); i++)
		{
			if (cond_strs[i] != "")
			{
				conds.push_back(InCond(cond_strs[i]));
			}
		}
	}
	bool rslt = true;
	i64 cur_block_size = query_context->getBlockSize();	
	aos_assert_r(cur_block_size >= 0,false);
	i64 num_in_rslt = 0;

	i64 total_in_rslt = query_context->getTotalDocInRslt();

	bool count_total = (total_in_rslt == 0);
	bool start_detect = false;

	OmnString cur_in_str_value = query_context->getCrtValue();
	if(cur_in_str_value != "")
	{
		count_total = false;
	}
	bool found_cur = false;
	u64 cmp_len = rdata->getCmpLen();

	for (u64 cur_pos = 0; cur_pos < conds.size(); cur_pos++)
	{
		if(cur_in_str_value != "")
		{
			if(!found_cur)
			{
				// 2 cases, 1. eq, 2, not in
				if(conds[cur_pos].mOpr == eAosOpr_eq &&
						(AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue1, mIsNumAlpha,cmp_len) != 0))
				{
					continue;
				}
				if(conds[cur_pos].mOpr == eAosOpr_lt &&
					(AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue1, mIsNumAlpha,cmp_len) >= 0))
				{
					if(!query_context->isReverse())
					{
						continue;
					}
				}
				if(conds[cur_pos].mOpr == eAosOpr_range_gt_lt)
				{
					if(query_context->isReverse() && 
						AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue1, mIsNumAlpha,cmp_len) <= 0)
					{
						continue;
					}
					
					if(!query_context->isReverse() &&
						AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue2, mIsNumAlpha,cmp_len) >= 0)
					{
						continue;
					}
				}
				
				if(query_context->isReverse() && 
					conds[cur_pos].mOpr == eAosOpr_gt &&
					AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue1, mIsNumAlpha,cmp_len) <= 0)
				{
					continue;
				}
				
			}
			found_cur = true;
		}

		query_context2->setOpr(conds[cur_pos].mOpr);
		query_context2->setStrValue(conds[cur_pos].mValue1);
		query_context2->setStrValue2(conds[cur_pos].mValue2);
		query_context2->setBlockSize(cur_block_size);
		query_context2->setFinished(false);
		query_context2->setTotalDocInRslt(0);
		if(start_detect)
		{
			query_context2->setDetect(true);
		}
		if (query_rslt)
		{
			query_rslt2 = AosQueryRsltObj::getQueryRsltStatic();
			query_rslt2->setWithValues(query_rslt->isWithValues());
		}
		if (orig_bitmap) query_bitmap2 = orig_bitmap->clone(rdata);

		rslt = queryNewPriv(query_rslt2, query_bitmap2, query_context2, rdata);
		aos_assert_r(rslt, false);

		total_in_rslt += query_context2->getTotalDocInRslt();	
		if(start_detect)
		{
			continue;
		}
		if (query_rslt2)
		{
			num_in_rslt = query_rslt2->getNumDocs();
			query_rslt->mergeFrom(query_rslt2);
		}
		else
		{
			num_in_rslt = query_bitmap2->getNumBits();
			aos_assert_r(query_bitmap, false);

			query_bitmap->orBitmap(query_bitmap2);
		}
		
		if (!query_context2->finished())
		{
			query_context->setFinished(false);
			query_context->setBackward(query_context2);
			if(count_total)
			{
				query_context2->setCrtValue("");
				query_context2->setCrtDocid(0);
				start_detect = true;
				continue;
			}
			return true;
		}
		else if (cur_block_size > 0 && num_in_rslt >= cur_block_size)
		{
			if (cur_pos == conds.size() - 1)
			{
				query_context->setFinished(query_context2->finished());
			}
			else
			{
				query_context->setFinished(false);
			}
			query_context->setBackward(query_context2);

			if(query_context2->finished())
			{
				if(conds[cur_pos].mOpr == eAosOpr_lt)
				{
					query_context->setCrtValue(conds[cur_pos].mValue1);
					query_context->setCrtDocid(1);
				}
				if(conds[cur_pos].mOpr == eAosOpr_gt)
				{
					query_context->setCrtValue(conds[cur_pos].mValue1);
					query_context->setCrtDocid(1);
				}
				if(conds[cur_pos].mOpr == eAosOpr_range_gt_lt)
				{
					if(query_context->isReverse())
					{
						query_context->setCrtValue(conds[cur_pos].mValue1);	
					}
					else
					{
						query_context->setCrtValue(conds[cur_pos].mValue2);
					}
					query_context->setCrtDocid(1);
				}
				if(conds[cur_pos].mOpr == eAosOpr_eq && cur_pos < conds.size() - 1)
				{
					query_context->setCrtValue(conds[cur_pos+1].mValue1);
					if(query_context->isReverse())
					{
						query_context->setCrtDocid(0x7fffffffffffffff);
					}
					else
					{
						query_context->setCrtDocid(1);
					}
				}
			}


			if(count_total)
			{
				query_context2->setCrtValue("");
				query_context2->setCrtDocid(0);
				start_detect = true;
				continue;
			}
			return true;
		}
		
		// else continue;
		if (cur_block_size > 0)
		{
			cur_block_size -= num_in_rslt;
		}
		
		if(conds[cur_pos].mOpr == eAosOpr_eq)
		{
			query_context2->setCrtValue("");
			query_context2->setCrtDocid(0);
		}
	}	

	if(!start_detect)
	{
		query_context->setFinished(true);
		query_context->setBackward(query_context2);
	}
	if(count_total)
	{
		query_context->setTotalDocInRslt(total_in_rslt);
	}
	return true;
}


bool
AosIILBigStr::bitmapQueryNewInPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt && query_bitmap, false);

	AosQueryContextObjPtr query_context2 = query_context->clone();
	AosBitmapObjPtr query_bitmap2;
	AosQueryRsltObjPtr query_rslt2;

	OmnString str_value = query_context->getStrValue();
	vector<OmnString> cond_strs;
	AosSplitStr(str_value, ";", cond_strs, eMaxNumOprIn);
	aos_assert_r(cond_strs.size() > 0 && cond_strs.size() < eMaxNumOprIn, false);

	vector<InCond> conds;
	for (u64 i = 0; i < cond_strs.size(); i++)
	{
		if (cond_strs[i] != "")
		{
			conds.push_back(InCond(cond_strs[i]));
		}
	}	

	bool rslt = true;
	i64 cur_block_size = query_context->getBlockSize();
	aos_assert_r(cur_block_size >= 0,false);
	i64 total_in_rslt = query_context->getTotalDocInRslt();

	bool count_total = (total_in_rslt == 0);
	bool start_detect = false;

	OmnString cur_in_str_value = query_context->getCrtValue();
	if(cur_in_str_value != "")
	{
		count_total = false;
	}
	bool found_cur = false;
	u64 cmp_len = rdata->getCmpLen();

	for (u64 cur_pos = 0; cur_pos < conds.size(); cur_pos++)
	{
		if(cur_in_str_value != "")
		{
			if(!found_cur)
			{
				// 2 cases, 1. eq, 2, not in
				if(conds[cur_pos].mOpr == eAosOpr_eq &&
						(AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue1, mIsNumAlpha,cmp_len) != 0))
				{
					continue;
				}
				if(conds[cur_pos].mOpr == eAosOpr_lt &&
					(AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue1, mIsNumAlpha,cmp_len) >= 0))
				{
					continue;
				}
				if(conds[cur_pos].mOpr == eAosOpr_range_gt_lt &&
					(AosIILUtil::valueNMatch(cur_in_str_value,conds[cur_pos].mValue2, mIsNumAlpha,cmp_len) >= 0))
				{
					continue;
				}
				
			}
			found_cur = true;
		}
		query_context2->setOpr(conds[cur_pos].mOpr);
		query_context2->setStrValue(conds[cur_pos].mValue1);
		query_context2->setStrValue2(conds[cur_pos].mValue2);
		query_context2->setBlockSize(cur_block_size);
		query_context2->setFinished(false);
		query_context2->setTotalDocInRslt(0);
		query_context2->setNumInRslt(0);
		if(start_detect)
		{
			query_context2->setDetect(true);
		}

		query_rslt2 = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt2->setWithValues(query_rslt->isWithValues());
		query_bitmap2 = AosGetBitmap();

		rslt = bitmapQueryNewPriv(query_rslt2, query_bitmap2, query_context2, rdata);
		aos_assert_r(rslt, false);
		
		total_in_rslt += query_context2->getTotalDocInRslt();
		if(start_detect)
		{
			continue;
		}
		query_rslt->mergeFrom(query_rslt2);
		query_bitmap->orBitmap(query_bitmap2);
		
		if (!query_context2->finished())
		{
			query_context->setFinished(false);
			query_context->setBackward(query_context2);
			if(count_total)
			{
				query_context2->setCrtValue("");
				query_context2->setCrtDocid(0);
				start_detect = true;
				continue;
			}
			return true;
		}
		else if (cur_block_size > 0 && query_context2->getNumInRslt() >= cur_block_size)
		{
			if (cur_pos == conds.size() - 1)
			{
				query_context->setFinished(query_context2->finished());
			}
			else
			{
				query_context->setFinished(false);
			}
			query_context->setBackward(query_context2);


			if(query_context2->finished())
			{
				if(conds[cur_pos].mOpr == eAosOpr_lt)
				{
					query_context->setCrtValue(conds[cur_pos].mValue1);
					query_context->setCrtDocid(1);
				}
				if(conds[cur_pos].mOpr == eAosOpr_range_gt_lt)
				{
					query_context->setCrtValue(conds[cur_pos].mValue2);
					query_context->setCrtDocid(1);
				}
				if(conds[cur_pos].mOpr == eAosOpr_eq && cur_pos < conds.size() - 1)
				{
					query_context->setCrtValue(conds[cur_pos+1].mValue1);
					query_context->setCrtDocid(1);
				}
			}

			if(count_total)
			{
				query_context2->setCrtValue("");
				query_context2->setCrtDocid(0);
				start_detect = true;
				continue;
			}
			return true;
		}

		// else continue;
		if (cur_block_size > 0)
		{
			cur_block_size -= query_context2->getNumInRslt();
		}
		if(conds[cur_pos].mOpr == eAosOpr_eq)
		{
			query_context2->setCrtValue("");
			query_context2->setCrtDocid(0);
		}
	}	

	if(!start_detect)
	{
		query_context->setFinished(true);
		query_context->setBackward(query_context2);
	}
	if(count_total)
	{
		query_context->setTotalDocInRslt(total_in_rslt);
	}
	return true;
}


bool
AosIILBigStr::bitmapRsltQueryNewInPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt && query_bitmap, false);

	AosQueryContextObjPtr query_context2 = query_context->clone();

	OmnString str_value = query_context->getStrValue();
	vector<OmnString> cond_strs;
	AosSplitStr(str_value, ";", cond_strs, eMaxNumOprIn);
	aos_assert_r(cond_strs.size() > 0 && cond_strs.size() < eMaxNumOprIn, false);

	vector<InCond> conds;
	for (u64 i = 0; i < cond_strs.size(); i++)
	{
		if (cond_strs[i] != "")
		{
			conds.push_back(InCond(cond_strs[i]));
		}
	}

	bool rslt = true;
	
	for (u64 cur_pos = 0; cur_pos < conds.size(); cur_pos++)
	{
		query_context2->setOpr(conds[cur_pos].mOpr);
		query_context2->setStrValue(conds[cur_pos].mValue1);
		query_context2->setStrValue2(conds[cur_pos].mValue2);
		query_context2->setFinished(false);
		query_context2->setEstimate(false);

		rslt = bitmapRsltQueryNewPriv(query_rslt, query_bitmap, query_context2, rdata);
		aos_assert_r(rslt, false);

		if (query_rslt->getNumDocs() >= query_bitmap->getNumBits())
		{
			// the query rslt is full
			break;
		}
	}	

	return true;
}


bool
AosIILBigStr::copyDataSingleWithThrd(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		AosIILIdx &cur_iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(start <= end, false);

	i64 size = end - start + 1;
	aos_assert_r(size >= AosIILMgr::smCopyDataWithThrdIILSize, false);

	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	aos_assert_r(size > 0,false);
	i64 single_size = size / AosIILMgr::smCopyDataWithThrdNum;
	i64 cur_start = start;
	i64 cur_end = end;
	for (int cur_thrd=0; cur_thrd < AosIILMgr::smCopyDataWithThrdNum; cur_thrd++)
	{
		cur_end = cur_start + single_size - 1;
		if (cur_thrd == AosIILMgr::smCopyDataWithThrdNum - 1) cur_end = end;

		AosIILBigStrPtr thisptr(this, false);
		runner = OmnNew AosIILBigStrCopyDataSingleThrd(thisptr,
			query_rslt, query_bitmap, query_context, cur_start, cur_end, rdata);
		runners.push_back(runner);
		cur_start += single_size;
	}

	AosIILMgr::smThreadPool->procSync(runners);

	u64 pos = 0;
	i64 num_docs = 0;
	AosBitmapObjPtr rslt_bitmap;
	AosIILBigStrCopyDataSingleThrd *thrd;
	bool onlyBitmap = (!query_rslt && query_bitmap);
	bool reverse = query_context->isReverse();
	if (reverse)
	{
		for (int cur_thrd=AosIILMgr::smCopyDataWithThrdNum - 1; cur_thrd >= 0; cur_thrd--)
		{
			thrd = dynamic_cast<AosIILBigStrCopyDataSingleThrd *>(runners[cur_thrd].getPtr());
			rslt_bitmap = thrd->mRsltBitmap;
			
			rslt_bitmap->reset();
			while (rslt_bitmap->prevDocid(pos)) 
			{
				cur_iilidx.setIdx(0, pos);

				if (onlyBitmap)
				{
					query_bitmap->appendDocid(mDocids[pos]);
				}
				else
				{
					query_rslt->appendDocid(mDocids[pos]);
					if (query_rslt->isWithValues())
					{
						query_rslt->appendStrValue(mValues[pos]);
					}
				}

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
		}

		num_docs = getRsltNumDocs(query_rslt, query_bitmap);
		aos_assert_r(num_docs >= 0, false);

		if (!query_context->isFull(num_docs))
		{
			cur_iilidx.setIdx(0, start);
		}
	}
	else
	{
		for (int cur_thrd=0; cur_thrd < AosIILMgr::smCopyDataWithThrdNum; cur_thrd++)
		{
			thrd = dynamic_cast<AosIILBigStrCopyDataSingleThrd *>(runners[cur_thrd].getPtr());
			rslt_bitmap = thrd->mRsltBitmap;
			
			rslt_bitmap->reset();
			while (rslt_bitmap->nextDocid(pos))
			{
				cur_iilidx.setIdx(0, pos);

				if (onlyBitmap)
				{
					query_bitmap->appendDocid(mDocids[pos]);
				}
				else
				{
					query_rslt->appendDocid(mDocids[pos]);
					if (query_rslt->isWithValues())
					{
						query_rslt->appendStrValue(mValues[pos]);
					}
				}

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					return true;
				}
			}
		}

		num_docs = getRsltNumDocs(query_rslt, query_bitmap);
		aos_assert_r(num_docs >= 0, false);

		if (!query_context->isFull(num_docs))
		{
			cur_iilidx.setIdx(0, end);
		}
	}

	return true;
}


bool
AosIILBigStr::copyDataSingleThrdSafe(
		AosIILBigStrCopyDataSingleThrd *thrd,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		const AosRundataPtr &rdata)
{
	thrd->mRsltBitmap = AosBitmapMgrObj::getBitmapStatic();
	aos_assert_r(query_context, false);
	aos_assert_r(start >= 0 && start <= end && end < mNumDocs, false);

	bool withCheck = (query_rslt && query_bitmap);
	bool hasFilter = query_context->hasFilter();

	for (i64 i=start; i<=end; i++)
	{
		if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
		{
			continue;
		}

		if (hasFilter && !query_context->evalFilter(
			mValues[i], mDocids[i], rdata))
		{
			continue;	
		}
			
		thrd->mRsltBitmap->appendDocid(i);
	}

	return true;
}

