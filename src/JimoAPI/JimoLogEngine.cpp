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
//
// Modification History:
// 2015/03/17 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
/*
#include "JimoAPI/JimoLogEngine.h"

#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCall.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/LogEngineObj.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
namespace Jimo
{

LogEngine gLogEngine;

bool
LogEngine::addLog(
		AosRundata *rdata,
		const OmnString &log_name,
		AosBSON *log)
{
	aos_assert_rr(log_name != "", rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsyncWrite(rdata,
		"AosLogEngineSvr", JimoFunc::eMethodAddLog,
		log_name, gRepPolicyMgr.getLogRepPolicy(), gClusterMgr,getLogEngineCluster(rdata));

	AosBuff *buff = log->getBuffRaw();
	jimo_call->arg(AosFN::eLogName, log_name);
	jimo_call->arg(AosFN::eData, buff->data(), buff->length());
	jimo_call->makeCall(rdata);

	return 0;
}


bool
LogEngine::addLog(
		AosRundata *rdata,
		const OmnString &log_name,
		AosBSON *log, 
		u64 &logid)
{
	aos_assert_rr(log_name != "", rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncWrite(rdata,
		"AosLogEngineSvr", eMethodAddLog,
		log_name, gRepPolicyMgr.getLogRepPolicy(), gClusterMgr.getLogEngineCluster(rdata));

	AosBuff *buff = log->getBuffRaw();
	jimo_call->arg(AosFN::eLogName, log_name);
	jimo_call->arg(AosFN::eData, buff->data(), buff->length());
	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess())
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	logid = jimo_call->getU64(rdata, AosFN::eDocid, 0);
	if (logid == 0)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	return true;
}


};

*/
