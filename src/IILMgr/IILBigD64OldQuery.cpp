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
#include "IILMgr/IILBigD64.h"

#include "IILMgr/IILMgr.h"


bool
AosIILBigD64::queryRangePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 04/18/2012
	// Do we need this (old) query function???????????????????
//shawn single function should be isolated
// need to verify start/end 
	aos_assert_r(query_context, false);
	AosOpr startOpr = query_context->getOpr();
	AosOpr endOpr;
	
	if (startOpr == eAosOpr_range_ge_le)
	{
	    startOpr = eAosOpr_ge;
	    endOpr = eAosOpr_le;
	}
	else if (startOpr == eAosOpr_range_ge_lt)
	{
	    startOpr = eAosOpr_ge;
	    endOpr = eAosOpr_lt;
	}
	else if (startOpr == eAosOpr_range_gt_le)
	{
	    startOpr = eAosOpr_gt;
	    endOpr = eAosOpr_le;
	}
	else if (startOpr == eAosOpr_range_gt_lt)
	{
	    startOpr = eAosOpr_gt;
	    endOpr = eAosOpr_lt;
	}
	else
	{
	    OmnAlarm << "Operation Error, not range opr:" << AosOpr_toStr(startOpr) << enderr;
	}
	
	aos_assert_r(startOpr == eAosOpr_gt || startOpr == eAosOpr_ge, false);
	aos_assert_r(endOpr == eAosOpr_lt || endOpr == eAosOpr_le, false);
	d64 startValue = query_context->getStrValue().toD64(0);
	d64 endValue = query_context->getStrValue2().toD64(0);

	bool rslt = false;
	u64 docid;
	bool isunique = false;

	AosIILIdx start_idx;
	AosIILIdx end_idx;
	if (isSingleIIL())
	{
		rslt = nextDocidPrivFind(start_idx, startValue, startOpr, docid, isunique, false, rdata);
		aos_assert_r(rslt, false);
	
		rslt = nextDocidPrivFind(end_idx, endValue, endOpr, docid, isunique, true, rdata);
		aos_assert_r(rslt, false);

		// copy data
	//	rslt = copyDocidsPriv(query_rslt, query_bitmap, idx_start, idx_end);
	//	aos_assert_r(rslt, false);

//error need fix.
		return false;
	}

	rslt = nextDocidPrivFind(start_idx, startValue, startOpr, docid, isunique, false, rdata);
	if (!rslt)
	{
		// This means that it did not find anything.
		return true;
	}
	
	rslt = nextDocidPrivFind(end_idx, endValue, endOpr, docid, isunique, true, rdata);
	if (!rslt)
	{
		OmnAlarm << "Internal error: " << rslt << ":" << end_idx.toString() << enderr;
		return false;
	}
	
//error need fix.
	return false;
	/*
	// count num of docs
	int num_docs = 0;
	for(int i=iilidx_start; i<iilidx_end; i++)
	{
		num_docs += mNumEntries[i];
	}
	num_docs += idx_end + 1;
	num_docs -= idx_start;

	// copy data
	AosIILBigD64Ptr subiil;
	int startpos = -10;
	int endpos = -10;
	for(int i=iilidx_start; i<=iilidx_end; i++)
	{
		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(), false);

		if (i == iilidx_start)
		{
			startpos = idx_start;
		}
		else
		{
			startpos = 0;
		}

		if (i == iilidx_end)
		{
			endpos = idx_end;
		}
		else
		{
			endpos = -10;
		}

		if (i == 0)
		{
			rslt = subiil->copyDocidsPriv(query_rslt, query_bitmap, startpos, endpos);
			aos_assert_r(rslt, false);
		}
		else
		{
			rslt = subiil->copyDocidsSafe(query_rslt, query_bitmap, startpos, endpos);
			aos_assert_r(rslt, false);
		}
	}
	return true;
	*/
}


//////////////////////////////////////////////////////////////////////////
//			Below are root level member functions						//
//////////////////////////////////////////////////////////////////////////

bool
AosIILBigD64::queryPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}





