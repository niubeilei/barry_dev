////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/02/14 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/StatQueryAnalyzerObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/QueryProcCallback.h"


AosStatQueryAnalyzerObjPtr AosStatQueryAnalyzerObj::smAnalyzer;


bool
AosStatQueryAnalyzerObj::runQueryStatic(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &req)
{
	aos_assert_rr(smAnalyzer, rdata, false);
	return smAnalyzer->analyzeReq(rdata, req);
}

	
bool
AosStatQueryAnalyzerObj::generateRsltStatic(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req,
		OmnString &contents)
{
	aos_assert_rr(smAnalyzer, rdata, false);
	return smAnalyzer->generateRslt(rdata, req, contents);
}


