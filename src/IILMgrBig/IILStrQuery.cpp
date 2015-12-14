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
#include "IILMgrBig/IILStr.h"

#include "API/AosApi.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryRsltObj.h"


bool
AosIILStr::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->copyFrom(query_rslt);
	bool rslt = queryNewPriv(query_rslt, query_bitmap, query_context, false, rdata);
	query_context->copyTo(query_rslt);	
	return rslt;
}


bool
AosIILStr::queryNewPriv(
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
	//	1. opr == like
	//	2. opr == not equal
	AosOpr opr = query_context->getOpr();
	if(opr == eAosOpr_like)
	{
		return queryNewLikePriv(query_rslt, query_bitmap, query_context, iilmgrlock, rdata);
	}
	else if(opr == eAosOpr_ne)
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
	bool rslt = queryPagingProc(query_rslt,	query_bitmap, query_context, block_start_iilidx, block_end_iilidx, has_data, iilmgrlock, rdata);
	aos_assert_r(rslt, false);

	if (!has_data)
	{
		// ken 2012/06/08
		// queryrslt should be empty, but it may be not
		// needfix 
		if (query_rslt) query_rslt->clean();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	//2. copy the data
	rslt = copyData(query_rslt, query_bitmap, query_context, block_start_iilidx, block_end_iilidx, iilmgrlock, rdata);	
	query_context->setTotalDocInIIL(mNumDocs);
	return rslt;
}


bool
AosIILStr::queryNewLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool
AosIILStr::queryNewNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool
AosIILStr::queryPagingProc(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
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
	has_data = true;
	if (query_context->finished())
	{
		OmnAlarm << "Querycontext incorrect" << enderr;
		has_data = false;
		return true;
	}

	bool need_paging = true;
	if(query_rslt && query_bitmap)
	{
		aos_assert_r(!query_bitmap->isEmpty(),false);
		need_paging  = false;
		query_context->setFinished(true);
	}
	if(!query_rslt && query_bitmap)
	{
		aos_assert_r(query_bitmap->isEmpty(),false);
		need_paging  = false;
		query_context->setFinished(true);
	}

	bool reverse = query_context->isReverse();
	AosOpr opr1 = query_context->getOpr();
	AosOpr opr2 = opr1;
	OmnString str_value = query_context->getStrValue();
	OmnString str_value2 = str_value; 

	if(opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt <= 0, false);
	}

    if(opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if(opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if(opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }
	
	//1. get the start idx/iilidx
	u64 cur_docid_0 = 0;
	bool isunique = false;
	bool rslt = nextDocidPrivFind(start_iilidx, str_value, opr1, cur_docid_0, isunique, false, rdata);
	aos_assert_r(rslt, false);

	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		has_data = false;
		return true;
	}
	
	//2. get the end idx/iilidx	
	rslt = nextDocidPrivFind(end_iilidx, str_value2, opr2, cur_docid_0, isunique, true, rdata); 
	aos_assert_r(rslt, false);
	
	rslt = end_iilidx.notFound(mLevel);
	if(rslt)
	{
		aos_assert_r(opr1!=opr2, false);
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
	if(query_context->getTotalDocInRslt() == 0)
	{
		query_context->setTotalDocInRslt(countNumRec(start_iilidx, end_iilidx, iilmgrlock, rdata));
	}
	//3. get the cur idx/iilidx, to fix start_idx/iilidx & end_idx/iilidx
	AosIILIdx cur_iilidx = query_context->getIILIndex2();
	
	if(!need_paging) return true;

	if (!cur_iilidx.isStart(mLevel))
	{
		// need to change current position
		OmnString cur_value = query_context->getCrtValue();
		u64 cur_docid = query_context->getCrtDocid();

		// It moves the 'cursor' to 'cur_iilidx' (from starting).
		nextIndexFixErrorIdx(cur_iilidx, cur_value, cur_docid, reverse, has_data, iilmgrlock, rdata);
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
	if (AosIILIdx::cmpPos(start_iilidx, end_iilidx) > 0)
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

	if(!need_paging) return true;

	//4. get the block start/end  idx/iilidx
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
		rslt = queryPagingProcRec(
			query_context, start_iilidx, end_iilidx,
			bsize, reverse, false, iilmgrlock, rdata);
		aos_assert_r(rslt, false);
		
		// Ken Lee, 2013/01/16
		aos_assert_r(bsize == 0, false);
		
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

/*
bool
AosIILStr::copyDataWithCheckRec(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		AosIILIdx &crt_iilidx,
		bool &finished,
		AosIILUtil::CopyType copytype,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return copyDataWithCheckRec(
					query_rslt,
					query_bitmap,
					query_context,
					start_iilidx,
					end_iilidx,
					crt_iilidx,
					finished,
					copytype,
					iilmgrlock, 
					rdata);
	}

	// not leaf
	cur_iilidx.setIdx(mLevel, i);
	bool reverse = query_context->isReverse();
	if(reverse)
	{
		
	}
}
	
bool
AosIILStr::copyDataWithCheck(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	// root function
	AosIILIdx cur_iilidx;
	bool finished = false;
	
	return 	copyDataWithCheckRec(
		query_rslt,
		query_bitmap,
		query_context,
		start_iilidx,
		end_iilidx,
		cur_iilidx,
		finished,
		AosIILUtil::eCopyAll,
		iilmgrlock, 
		rdata);
}

*/
bool
AosIILStr::copyDataSingle(
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

	bool reverse = query_context->isReverse();
	int start = start_iilidx.getIdx(0);
	int end = end_iilidx.getIdx(0);
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
		end = mNumDocs - 1;
	}

	aos_assert_r(start >= 0 && end <= (int)mNumDocs-1 && start <= end, false);
	
	int size = end - start + 1;
	bool isGrouping = query_context->isGrouping();
	if(isGrouping)
	{
		aos_assert_r(query_rslt, false);
		if(reverse)
		{
			for(int i = end;i >= start;i--)
			{
				if(query_rslt && query_bitmap && !query_bitmap->checkDoc(mDocids[i]))
				{
					continue;
				}
				cur_iilidx.setIdx(0, i);
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
				if(query_rslt && query_bitmap && !query_bitmap->checkDoc(mDocids[i]))
				{
					continue;
				}
				cur_iilidx.setIdx(0, i);
				query_context->appendEntry(mValues[i],mDocids[i],cur_iilidx,query_rslt);
				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
		}
		return true;
	}
	bool withCheck = (query_rslt && query_bitmap);
	if(withCheck)
	{
		aos_assert_r(query_rslt, false);
		if(reverse)
		{
			for(int i = end;i >= start;i--)
			{
				if(query_bitmap->checkDoc(mDocids[i]))
				{
					cur_iilidx.setIdx(0, i);
					query_rslt->appendDocid(mDocids[i]);
					if(query_rslt->isWithValues())
					{
						query_rslt->appendStrValue(mValues[i]);
					}
					if(query_rslt && query_context->isFull(query_rslt))
					{
						return true;
					}
				}
			}
		}
		else
		{
			for(int i = start;i <= end;i++)
			{
				if(query_bitmap->checkDoc(mDocids[i]))
				{
					cur_iilidx.setIdx(0, i);
					query_rslt->appendDocid(mDocids[i]);
					if(query_rslt->isWithValues())
					{
						query_rslt->appendStrValue(mValues[i]);
					}
					if(query_rslt && query_context->isFull(query_rslt))
					{
						return true;
					}
				}
			}
		}
		return true;
	}
	
	if(query_rslt)
	{
		if(reverse)
		{
			query_rslt->appendBlockRev(&(mDocids[start]), size);
		}
		else
		{
			query_rslt->appendBlock(&(mDocids[start]), size);
		}
	}
	
	if(query_bitmap)
	{
		query_bitmap->appendBlock(&(mDocids[start]), size);
	}
	
	if(query_rslt && query_rslt->isWithValues())
	{
		// copy values
		if(reverse)
		{
			for(int i = end; i >= start; i--)
			{
				query_rslt->appendStrValue(mValues[i]);
			}
		}
		else
		{
			for(int i = start; i <= end; i++)
			{
OmnScreen << " %%%%%%%%%% i:" << i << " iilid:" << mIILID << " mVlaues : " << mValues[i] << "  mDocids : " << mDocids[i] << endl;	
				query_rslt->appendStrValue(mValues[i]);
			}
		}
	}
	return true;
}


bool
AosIILStr::copyDataRec(
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
		return copyDataSingle(
			query_rslt, query_bitmap, query_context, start_iilidx,
			end_iilidx, cur_iilidx, copytype, iilmgrlock, rdata);
	}
	
	AosIILStrPtr subiil;
	bool rslt = false;
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);
//	bool isGrouping = query_context->isGrouping();
	bool reverse = query_context->isReverse();
	if(!reverse)
	{
		if(copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for(int i = 0; i < (int)mNumSubiils; i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyFromHead)
		{
			aos_assert_r(end >= 0 && end <= (int)mNumSubiils-1, false);
			// copy each subiils
			for(int i = 0; i < end; i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromHead, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end, iilmgrlock, rdata);

			return true;
		}
		
		if(copytype == AosIILUtil::eCopyToTail)
		{
			aos_assert_r(start >= 0 && start <= (int)mNumSubiils-1, false);
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToTail, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start, iilmgrlock, rdata);

			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}

			for(int i = start+1;i < (int)mNumSubiils;i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= (int)mNumSubiils-1 && start <= end, false);
			
			if(start == end)
			{
				subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyNormal, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(start, iilmgrlock, rdata);

				return true;
			}
			
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToTail, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start, iilmgrlock, rdata);

			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}

			for(int i = start+1;i < end;i++)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromHead, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end, iilmgrlock, rdata);

			return true;
		}
	}
	else// reverse
	{
		if(copytype == AosIILUtil::eCopyAll)
		{
			// copy each subiils
			for(int i = mNumSubiils-1 ;i >= 0 ;i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			return true;
		}
		
		if(copytype == AosIILUtil::eCopyToHead)
		{
			aos_assert_r(end >= 0 && end <= (int)mNumSubiils-1, false);
			// copy each subiils
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToHead, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end, iilmgrlock, rdata);

			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}

			for(int i = end-1;i >= 0;i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

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
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			// copy each subiils
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromTail, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start, iilmgrlock, rdata);

			return true;
		}
		
		if(copytype == AosIILUtil::eCopyNormal)
		{
			aos_assert_r(start >= 0 && end <= (int)mNumSubiils-1 && start <= end,false);
			
			if(start == end)
			{
				subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, start);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx, 
					AosIILUtil::eCopyNormal, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(start, iilmgrlock, rdata);

				return true;
			}
			
			// copy each subiils
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, end);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyFromTail, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(end, iilmgrlock, rdata);

			
			if(query_rslt && query_context->isFull(query_rslt))
			{
				return true;
			}
			
			for(int i = end -1;i > start;i--)
			{
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);
				aos_assert_r(subiil, false);
				cur_iilidx.setIdx(mLevel, i);
				rslt = subiil->copyDataRec(
					query_rslt, query_bitmap, query_context,
					start_iilidx, end_iilidx, cur_iilidx,
					AosIILUtil::eCopyAll, iilmgrlock, rdata);
				aos_assert_r(rslt, false);
				rslt = returnSubIILPriv(i, iilmgrlock, rdata);

				if(query_rslt && query_context->isFull(query_rslt))
				{
					return true;
				}
			}
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
			aos_assert_r(subiil, false);
			cur_iilidx.setIdx(mLevel, start);
			rslt = subiil->copyDataRec(
				query_rslt, query_bitmap, query_context,
				start_iilidx, end_iilidx, cur_iilidx,
				AosIILUtil::eCopyToHead, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			rslt = returnSubIILPriv(start, iilmgrlock, rdata);

			return true;
		}
	}
	return true;
}


bool
AosIILStr::copyData(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
/*	if(query_rslt && query_bitmap)
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
*/	
	aos_assert_r(query_context, false);
//	bool reverse = query_context->isReverse();
//	if(reverse)
//	{
//		query_rslt->setDataReverse(true);
//	}
	
	if(query_context->isGrouping())
	{
		aos_assert_r(query_rslt, false);
		query_context->appendEntryStrStart(query_rslt);
	}

	AosIILIdx cur_iilidx;
	bool rslt = copyDataRec(
		query_rslt, query_bitmap, query_context,
		start_iilidx, end_iilidx, cur_iilidx,
		AosIILUtil::eCopyNormal, iilmgrlock, rdata);
	if(query_context->isGrouping())
	{
		aos_assert_r(query_rslt, false);
		query_context->appendEntryStrFinish(query_rslt);
	}
	aos_assert_r(rslt, false);
	return true;
}


void
AosIILStr::nextIndexFixErrorIdx(
		AosIILIdx &cur_iilidx,
		const OmnString &cur_value,
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
			rslt = prevQueryPosRec(
				cur_iilidx, iilmgrlock, rdata);
			if(!rslt)
			{
				has_data = false;
				return;
			}				
		}
		else
		{
			// error handling, if the index is the last one???????????
			rslt = nextQueryPosRec(
				cur_iilidx, iilmgrlock, rdata);
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
AosIILStr::queryPagingProcRec(
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
		return queryPagingProcSingle(
			query_context, start_iilidx, end_iilidx,
			bsize, reverse, from_start, iilmgrlock, rdata);
	}
	//1. proc first subiil
	//2. proc other subiil
	//3. proc last subiil
	bool rslt = false;
	AosIILStrPtr subiil;
	if(reverse)
	{
		int cur_sub_idx = end_iilidx.getIdx(mLevel);
		if(from_start)
		{
			cur_sub_idx = mNumSubiils - 1;
		}
		else
		{
			subiil = getSubiilByIndexPriv(cur_sub_idx, iilmgrlock, rdata);
			aos_assert_r(subiil, false);

			// Handle the first subiil
			rslt = subiil->queryPagingProcRec(
				query_context, start_iilidx, end_iilidx,
				bsize, reverse, false, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			
			// mod by shawn 13-02-25  if(bsize <= 0)
			if(bsize <= 0 || cur_sub_idx == 0)
			{
				query_context->setIILIndex2(mLevel, cur_sub_idx);
				return true;
			}
			if(cur_sub_idx == 0)
			{
				query_context->setIILIndex2(mLevel, cur_sub_idx);
				return true;
			}
			cur_sub_idx--;
		}
		//ken 2013/01/02
		//while(cur_sub_idx >= 0)
		while(cur_sub_idx > 0)
		{
			if(mNumEntries[cur_sub_idx] >= bsize)
			{
				break;
			}
			bsize -= mNumEntries[cur_sub_idx];
			cur_sub_idx--;
		}
		aos_assert_r(cur_sub_idx >= 0, false);

		subiil = getSubiilByIndexPriv(cur_sub_idx, iilmgrlock, rdata);
		aos_assert_r(subiil, false);

		rslt = subiil->queryPagingProcRec(
			query_context, start_iilidx, end_iilidx,
			bsize, reverse, true, iilmgrlock, rdata);	
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
			aos_assert_r(subiil, false);

			rslt = subiil->queryPagingProcRec(
				query_context, start_iilidx, end_iilidx,
				bsize, reverse, false, iilmgrlock, rdata);	
			aos_assert_r(rslt, false);
			// mod by shawn 13-02-25  if(bsize <= 0)
			if(bsize <= 0 || cur_sub_idx == (int)mNumSubiils-1)
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
		//while(cur_sub_idx <= (int)mNumSubiils-1)
		while(cur_sub_idx < (int)mNumSubiils-1)
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
		aos_assert_r(subiil, false);

		// Handle the last one.
		rslt = subiil->queryPagingProcRec(
			query_context, start_iilidx, end_iilidx,
			bsize, reverse, true, iilmgrlock, rdata);	
		aos_assert_r(rslt, false);	
		
		// Ken Lee, 2013/01/16
		//aos_assert_r(bsize == 0, false);
		query_context->setIILIndex2(mLevel, cur_sub_idx);
		return true;
	}
	return true;
}


bool
AosIILStr::queryPagingProcSingle(
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
	query_context->setIILIndex2(0, cur_pos);
	query_context->setCrtValue(mValues[cur_pos]);
	query_context->setCrtDocid(mDocids[cur_pos]);
	bsize = 0;
	return true;
}


bool
AosIILStr::getSplitValuePriv(
		const int &num_blocks,
		const AosQueryRsltObjPtr &query_rslt,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(num_blocks > 1, false);
	int num_splitters = num_blocks-1;
	int cur_split_num = 0;
	int num_each_split = mNumDocs/num_blocks;
	int cur_doc_num = num_each_split;
	OmnString cur_value;
	return getSplitValueRecPriv(
		num_splitters, cur_split_num, num_each_split,
		cur_doc_num, cur_value, query_rslt, iilmgrlock, rdata);
}

bool
AosIILStr::getSplitValueRecPriv(
		int &num_splitters, 
		int &cur_split_num, 
		int &num_each_split, 
		int &cur_doc_num,
		OmnString &cur_value,
		const AosQueryRsltObjPtr &query_rslt,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return 	getSplitValueSinglePriv(
			num_splitters, cur_split_num, num_each_split,
			cur_doc_num, cur_value, query_rslt, iilmgrlock, rdata);
	}
	
	AosIILStrPtr subiil;
	bool rslt = false;
	for(int i = 0; i < (int)mNumSubiils;i++)
	{
		if(mNumEntries[i] < cur_doc_num)
		{
			cur_doc_num -= mNumEntries[i];
		}
		else
		{
			subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);
			aos_assert_r(subiil, false);
			rslt = subiil->getSplitValueRecSafe(
				num_splitters, cur_split_num, num_each_split,
				cur_doc_num, cur_value, query_rslt, iilmgrlock, rdata);
			aos_assert_r(rslt, false);
			if(cur_split_num >= num_splitters)
			{
				return true;
			}
		}
	}
	return true;
}

bool
AosIILStr::getSplitValueSinglePriv(
		int &num_splitters, 
		int &cur_split_num, 
		int &num_each_split, 
		int &cur_doc_num,
		OmnString &cur_value,
		const AosQueryRsltObjPtr &query_rslt,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	if((int)mNumDocs < cur_doc_num)
	{
		cur_doc_num -= mNumDocs;
		return true;
	}
	int cur_pos = 0;

	while(cur_doc_num + cur_pos <= (int)mNumDocs)
	{
		//1. get the cur_pos
		cur_pos += (cur_doc_num-1);
		cur_doc_num = 0;
		aos_assert_r(cur_pos < (int)mNumDocs,false);

		if(AosIILUtil::valueMatch(mValues[cur_pos],cur_value,mIsNumAlpha)==0)
		{
			if(AosIILUtil::valueMatch(mValues[mNumDocs-1],cur_value,mIsNumAlpha)==0)
			{
				return true;
			}
			while(cur_pos < (int)mNumDocs && AosIILUtil::valueMatch(mValues[cur_pos],cur_value,mIsNumAlpha)==0)
			{
				cur_pos ++;
			}
			aos_assert_r(cur_pos < (int)mNumDocs,false);
		}
		// append value,reset doc_num
		query_rslt->appendDocid(mDocids[cur_pos]);
		query_rslt->appendStrValue(mValues[cur_pos]);
		cur_value = mValues[cur_pos];
		cur_split_num ++;
		if(cur_split_num >= num_splitters)
		{
			return true;
		}
		cur_doc_num = num_each_split;		
	}
	
	
	return true;
}


bool	
AosIILStr::getSplitValuePriv(
		const AosQueryContextObjPtr &query_context,
		const int size,
		vector<AosQueryContextObjPtr> &contexts,
		const bool iilmgrlock,
		const AosRundataPtr &rdata)
{
	aos_assert_r(size > 0,false);
	AosOpr opr = query_context->getOpr();
	
	if((opr == eAosOpr_ne) || (opr == eAosOpr_like))	
	{
		OmnNotImplementedYet;
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return false;
	}
	
	AosOpr opr1 = opr;
	AosOpr opr2 = opr;
	OmnString str_value = query_context->getStrValue();
	OmnString str_value2 = str_value; 

	if(opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt <= 0, false);
	}

    if(opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if(opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if(opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

	AosIILIdx start_iilidx;
	AosIILIdx end_iilidx;
	
	// find start iilidx
	u64 cur_docid_0 = 0;
	bool isunique = false;
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
	if(rslt)
	{
		aos_assert_r(opr1 != opr2, false);                                                                                       
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}

	// count num
	int total = countNumRec(start_iilidx, end_iilidx, iilmgrlock, rdata);
	if(total / size < 1)
	{
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}
	int size2 = size;
	int each = total / size;
	int min_for_each = query_context->getPageSize();
	if(each < min_for_each)
	{
		size2 = total / min_for_each;
		if(size <= 1)
		{
			// return the copy of the query_context
			AosQueryContextObjPtr cur_context = query_context->clone();
			contexts.push_back(cur_context);
			return true;			
		}
		each = total / size2;
	}
	
	vector<OmnString> splitKeys;
	vector<OmnString> values;
	vector<OmnString>::iterator itr;
	int num_left = each;
	int saperator_left = size2 - 1;
	rslt = getSplitValueRec(
		AosIILUtil::eCopyNormal, start_iilidx, end_iilidx,
		each, num_left, saperator_left, splitKeys, iilmgrlock, rdata);
	aos_assert_r(rslt, false);
	
	AosQueryContextObj::SplitValueType svtype = query_context->getSplitValueType();
	OmnString svstr = query_context->getSplitValueStr();
	AosConvertAsciiBinary(svstr);

	OmnString vv;
	itr = splitKeys.begin();
	while(itr != splitKeys.end())
	{
		vv = getSVStr(*itr, svtype, svstr);
		if(vv != "") values.push_back(vv);
		itr++;
	}

	// remove no use saperator
	itr = values.begin(); 
	OmnString last_value;
	while (itr != values.end()) 
	{
		if(compWithSVType(*itr,str_value,svtype) || compWithSVType(*itr,str_value2,svtype) ||
				(itr != values.begin() && compWithSVType(*itr, last_value,svtype)))
		{
			values.erase(itr);
			continue;
		}
		last_value = *itr;
		itr++;
	}

	if(values.size() == 0)
	{
		// return the copy of the query_context
		AosQueryContextObjPtr cur_context = query_context->clone();
		contexts.push_back(cur_context);
		return true;
	}
		
	// proc the first one
	// clone query_context to query_contexts
	AosQueryContextObjPtr cur_context = query_context->clone();
	
	if(opr1 == eAosOpr_ge)
	{
		cur_context->setOpr(eAosOpr_range_ge_lt);
		cur_context->setStrValue(str_value);
		cur_context->setStrValue2(values[0]);
	}
	else if(opr1 == eAosOpr_gt)
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
	//ken 2012/08/08
	//for(u32 i = 1;i < (values.size()-1);i++)
	for(u32 i = 1;i < values.size();i++)
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
	
	if(opr1 == eAosOpr_lt)
	{
		cur_context->setOpr(eAosOpr_range_ge_lt);
		cur_context->setStrValue(values[values.size()-1]);
		cur_context->setStrValue2(str_value2);
	}
	else if(opr1 == eAosOpr_le)
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
AosIILStr::getSplitValueRec(
		const AosIILUtil::CopyType type,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const int each,
		int &num_left,
		int &saperator_num_left,
		vector<OmnString> &values,
		const bool iilmgrlock,
		const AosRundataPtr &rdata)
{
	AosIILStrPtr subiil;
	if(isLeafIIL())
	{
		return getSplitValueSingle(
			type, start_iilidx, end_iilidx, each, num_left,
			saperator_num_left, values, iilmgrlock, rdata);
	}
	
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);

	switch(type)
	{
	case AosIILUtil::eCopyAll :
		 {
			for(int i = 0;i < (int)mNumSubiils;i++)
			{
				if(num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
					
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, iilmgrlock, rdata);
				if(saperator_num_left <= 0) return true;
			}
		 }			
		 break;

	case AosIILUtil::eCopyFromHead :
		 {
			for(int i = 0;i < end-1;i++)
			{
				if(num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
				
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, iilmgrlock, rdata);
				if(saperator_num_left <= 0) return true;
			}
			
			//proc the last one
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyFromHead, start_iilidx, end_iilidx, each,
				num_left, saperator_num_left, values, iilmgrlock, rdata);
			if(saperator_num_left <= 0) return true;
		 }
		 break;

	case AosIILUtil::eCopyToTail :
		 {
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyToTail, start_iilidx, end_iilidx, each,
				num_left, saperator_num_left, values, iilmgrlock, rdata);
			if(saperator_num_left <= 0) return true;
			for(int i = start+1;i < (int)mNumSubiils;i++)
			{
				if(num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
					
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, iilmgrlock, rdata);
				if(saperator_num_left <= 0) return true;
			}
		 }
		 break;

	case AosIILUtil::eCopyNormal :
		 {
			if(start == end)
			{
				subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyNormal, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, iilmgrlock, rdata);
				return true;
			}
			
			// start != end
			subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyToTail, start_iilidx, end_iilidx, each,
				num_left, saperator_num_left, values, iilmgrlock, rdata);
			if(saperator_num_left <= 0) return true;

			for(int i = start+1;i < end-1;i++)
			{
				if(num_left > mNumEntries[i])
				{
					num_left -= mNumEntries[i];
					continue;
				}
					
				subiil = getSubiilByIndexPriv(i, iilmgrlock, rdata);		
				subiil->getSplitValueRec(
					AosIILUtil::eCopyAll, start_iilidx, end_iilidx, each,
					num_left, saperator_num_left, values, iilmgrlock, rdata);
				if(saperator_num_left <= 0) return true;
			}
			subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);		
			subiil->getSplitValueRec(
				AosIILUtil::eCopyFromHead, start_iilidx, end_iilidx, each, 
				num_left, saperator_num_left, values, iilmgrlock, rdata);
			if(saperator_num_left <= 0) return true;
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
AosIILStr::getSplitValueSingle(
		const AosIILUtil::CopyType type,
		const AosIILIdx &start_iilidx,
		const AosIILIdx &end_iilidx,
		const int each,
		int &num_left,
		int &saperator_num_left,
		vector<OmnString> &values,
		const bool iilmgrlock,
		const AosRundataPtr &rdata)
{
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);

	switch(type)
	{
		case AosIILUtil::eCopyAll     :
			start = 0;
			end = mNumDocs -1;
			break;
		case AosIILUtil::eCopyFromHead: 
			start = 0;
			break;
		case AosIILUtil::eCopyToTail  :
			end = mNumDocs -1;
			break;
		case AosIILUtil::eCopyNormal  :
			break;
		default:		
			break;
	}    
	
	int cur = start;
	while(end-cur+1 >= num_left)
	{
		// 1. shift num_left
		// 2. record value
		// 3. saperator_num_left --
		// 4. reset num_left
		cur += num_left;
		values.push_back(mValues[cur-1]);
		saperator_num_left --;
		if(saperator_num_left<= 0)return true;
		num_left = each;
	}
	
	// not enough for a piece
	num_left -= (end-cur+1);
	return true;
}         
          
bool	
AosIILStr::preQueryNEPriv(
		const AosQueryContextObjPtr &query_context,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	return false;
}

bool	
AosIILStr::preQueryPriv(
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
	OmnString str_value = query_context->getStrValue();
	OmnString str_value2 = str_value; 
	if(opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt <= 0, false);
	}

    if(opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if(opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if(opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

	//1. get the start idx/iilidx
	u64 cur_docid_0 = 0;
	bool isunique = false;

	bool rslt = nextDocidPrivFind(
		start_iilidx, str_value, opr1,
		cur_docid_0, isunique, false, rdata);
	aos_assert_r(rslt, false);
	rslt = start_iilidx.notFound(mLevel);
	if (rslt)
	{
		return true;
	}
	
	//2. get the end idx/iilidx	
						 
	rslt = nextDocidPrivFind(
		end_iilidx, str_value2, opr2,
		cur_docid_0, isunique, true, rdata);
	aos_assert_r(rslt, false);
	rslt = end_iilidx.notFound(mLevel);
	if(rslt)
	{
		aos_assert_r(opr1 != opr2, false);                                                                                       
		return true;
	}
	
	if(query_context->getTotalDocInRslt() == 0)
	{
		query_context->setTotalDocInRslt(countNumRec(start_iilidx, end_iilidx, iilmgrlock, rdata));
	}

	return rslt;
}

bool
AosIILStr::compWithSVType(
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
AosIILStr::getSVStr(
		const OmnString &key,
		AosQueryContextObj::SplitValueType svtype,
		const OmnString &sep)
{
	int idx = -1;
	int idx2 = -1;
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

