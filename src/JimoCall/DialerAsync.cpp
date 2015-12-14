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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/DialerAsync.h"

#include "CubeComm/CubeCommTcpClt.h"
#include "CubeComm/CubeCommTcpCltNew.h"
#include "CubeComm/CubeCommUdp.h"
#include "CubeComm/EndPointInfo.h"
#include "Debug/Except.h"
#include "JimoAPI/Jimo.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/EndPointMgr.h"
#include "Message/MsgId.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoCaller.h"
#include "UtilData/FN.h"


AosDialerAsync::AosDialerAsync(
		AosRundata *rdata, 
		const AosXmlTagPtr &cfg,
		const u32 dialer_id) 
:
AosJimoCallDialer(rdata, eAuto)
{
	mJimoCallID = ((u64)dialer_id) << 48;

	bool rslt = config(rdata, cfg);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosDialerAsync::~AosDialerAsync()
{
}


bool
AosDialerAsync::config(AosRundata *rdata, const AosXmlTagPtr &cfg)
{
	aos_assert_r(cfg, false);

	OmnString conn_type = cfg->getAttrStr("conn_type", "udp");
	OmnCommListenerPtr caller(this, false);
	if (conn_type == "udp")
	{
		mCubeComm = OmnNew AosCubeCommUdp(caller);
	}
	else if (conn_type == "tcp")
	{
		mCubeComm = OmnNew AosCubeCommTcpClt(caller);
	}
	else if (conn_type == "async")
	{
		mCubeComm = OmnNew AosCubeCommTcpCltNew(caller);
	}
	else
	{
		OmnAlarm << "type error" << enderr;
		return false;
	}

	mCubeCommRaw = mCubeComm.getPtr();
	return true;
}


u64
AosDialerAsync::nextJimoCallID(AosRundata *rdata)
{
	u64 jimocall_id = __sync_fetch_and_add(&mJimoCallID, 1);
	return jimocall_id;
}

//comment out the alarm for now
static int maxTotalAlarms = 0;

bool
AosDialerAsync::makeJimoCall(
		AosRundata *rdata,
		const AosEndPointInfo &endpoint,
		const AosJimoCallPtr &jimo_call,
		AosBuff *buff)
{
	u64 jimocall_id = jimo_call->getJimoCallID();

	mLockRaw->lock();
	mPendingCalls[jimocall_id] = jimo_call;
	mLockRaw->unlock();
	bool rslt = mCubeCommRaw->sendTo(rdata, endpoint, buff);
	//aos_assert_rr(rslt, rdata, false);
	if (!rslt)
	{ 
		if (maxTotalAlarms > 0) 
		{
			OmnAlarm<< "conn failed" << enderr ;
			maxTotalAlarms--;
		}
	}

	return true;
}


bool
AosDialerAsync::getTargetCubes(AosRundata *rdata, vector<AosCubeMapObj::CubeInfo> &cube_infos)
{
	OmnNotImplementedYet;
	return false;
}
