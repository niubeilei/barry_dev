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
// 2014/11/22 Created by Chen Ding
// 2015/03/14 Copied from DocPackage.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoRaftAPI.h"

#include "JimoAPI/JimoPackage.h"
#include "JimoAPI/JimoFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoCall/JimoCallSyncNorm.h"
#include "JimoCall/JimoCallAsyncNorm.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/JimoCaller.h"
#include "JimoCall/JimoCallerDummy.h"

static OmnString sgJimoRaftClassName = "AosRaftJimoCalls";

namespace Jimo
{

bool raftSendMsgSync(
		AosRundata *rdata, 
		u32 endPointId,
		const AosBuffPtr &buff)
{
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoRaftClassName, JimoFunc::eRaftRecvMsg,
		cluster, endPointId);

	jimo_call->arg(AosFN::eBuff, buff);
	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}

	AosValueRslt value;
	rslt = jimo_call->getFieldValue(rdata, AosFN::eReturnValue, value);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}

	//respStr not used for now
	OmnString respStr = value.getStr();
	return true;
}


bool raftSendMsgAsync(
		AosRundata *rdata, 
		u32 endPointId,
		const AosBuffPtr &buff)
{
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);

	AosJimoCallerPtr caller = OmnNew AosJimoCallerDummy();
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsyncNorm(rdata,
		sgJimoRaftClassName, JimoFunc::eRaftRecvMsg, caller,
		cluster, endPointId);

	jimo_call->arg(AosFN::eBuff, buff);
	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}

	return true;
}


bool raftSendMsgAsyncNoResp(
		AosRundata *rdata,
		u32 endPointId,
		const AosBuffPtr &buff)
{
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);

	AosJimoCallerPtr caller = OmnNew AosJimoCallerDummy();
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsyncNorm(rdata,
		sgJimoRaftClassName, JimoFunc::eRaftRecvMsg, caller,
		cluster, endPointId);

	jimo_call->arg(AosFN::eBuff, buff);
	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << enderr;
		return false;
	}
	u64 ullJimoCallID = jimo_call->getJimoCallID();
	cluster->eraseJimoCall(rdata, ullJimoCallID);
	return true;
}

};
