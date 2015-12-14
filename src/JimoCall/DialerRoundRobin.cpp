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
#include "JimoCall/DialerRoundRobin.h"
#include "Debug/Except.h"
#include "JimoAPI/Jimo.h"
#include "Message/MsgId.h"
#include "Rundata/Rundata.h"


AosDialerRoundRobin::AosDialerRoundRobin(
		AosRundata *rdata, 
		const AosXmlTagPtr &conf,
		const u32 dialer_id)
:
AosJimoCallDialer(rdata, eRoundRobin),
mEndPointIdx(0)
{
/*
	mJimoCallID = ((u64)dialer_id) << 48;
	if (!Jimo::getFrontends(rdata, mEndpoints))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}

	mCubeComm = OmnNew AosCubeComm(Jimo::getSelfEPID());
	OmnCommListenerPtr thisptr(this, false);
	mCubeComm->startReading(rdata, thisptr);
*/
}


AosDialerRoundRobin::~AosDialerRoundRobin()
{
	OmnDelete mCubeComm;
	mCubeComm = 0;
}


/*
u64
AosDialerRoundRobin::makeJimoCall(
		AosRundata *rdata, 
		const AosJimoCallPtr &call, 
		AosBuff *data, 
		u32 &from_epid,
		u32 &to_epid)
{
	u32 epid_idx = mEndpointIdx++;
	if (mEndpointIdx >= mEndpoints.size()) mEndpointIdx = 0;
	if (epid_idx >= mEndpoints.size()) epid_idx = 0;

	u64 jimocall_id = mJimoCallID++;
	from_epid = mSelfEPID;
	to_epid = mEndpoints[epid_idx].mEpid;
	rdata->serializeForJimoCall(data, jimocall_id, OmnMsgId::eJimoCallReq);
	mCubeComm->sendTo(rdata, to_epid, data);

	return jimocall_id;
}
*/
