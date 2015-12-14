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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiB.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"


int AosBinarySearchOnU32Array(
		AosRundata *rdata, 
		const u32 *array, 
		const int size, 
		const u32 value)
{
	OmnNotImplementedYet;
	return -1;
}

bool AosBitmapQueryColumn(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(query_context, false);
	AosIILClientObjPtr theobj = AosIILClientObj::getIILClient();
	aos_assert_r(theobj, false);
	return theobj->bitmapQueryNewSafe(iilname, query_rslt, query_bitmap, query_context, rdata);
}


bool AosBitmapRsltQueryColumn(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(query_context, false);
	AosIILClientObjPtr theobj = AosIILClientObj::getIILClient();
	aos_assert_r(theobj, false);
	return theobj->bitmapRsltQuerySafe(iilname, query_rslt, query_bitmap, query_context, rdata);
}

bool AosBatchAdd(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr theobj = AosIILClientObj::getIILClient();
	aos_assert_r(theobj, false);
	return theobj->BatchAdd(iilname, iiltype, entry_len, buff, executor_id, snap_id, task_docid, rdata);
}


bool AosBatchDel(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr theobj = AosIILClientObj::getIILClient();
	aos_assert_r(theobj, false);
	return theobj->BatchDel(iilname, iiltype, entry_len, buff, executor_id, snap_id, task_docid, rdata);

}

