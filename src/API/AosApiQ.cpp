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
// 05/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApi.h"

#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"


bool AosQueryColumn(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid > 0, false);
	aos_assert_r(query_context, false);
	AosIILClientObjPtr theobj = AosIILClientObj::getIILClient();
	aos_assert_r(theobj, false);
	bool rslt = theobj->querySafe(iilid, query_rslt, query_bitmap, query_context, rdata);
	if(!rslt)
	{
		OmnAlarm << "IIL " << iilid << " query failed, it may not exist(please check cube log)." << enderr;	
	}
	return rslt;
}


bool AosQueryColumn(
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
	bool rslt = theobj->querySafe(iilname, query_rslt, query_bitmap, query_context, rdata);
	if(!rslt)
	{
		OmnAlarm << "IIL " << iilname << " query failed, it may not exist(please check cube log)." << enderr;	
	}
	return rslt;
}


bool AosQueryColumnAsync(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(query_context, false);
	AosIILClientObjPtr theobj = AosIILClientObj::getIILClient();
	aos_assert_r(theobj, false);
	bool rslt = theobj->querySafe(iilname, query_rslt, query_bitmap, query_context, resp_caller, reqId, snapId, rdata);
	if(!rslt)
	{
		OmnAlarm << "IIL " << iilname << " query failed, something wrong." << enderr;	
	}
	return rslt;
}

