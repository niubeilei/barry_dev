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
#include "JimoCall/DialerManual.h"

#include "CubeComm/CubeCommTcpCltNew.h"
#include "CubeComm/CubeCommUdp.h"
#include "Debug/Except.h"
#include "JimoAPI/Jimo.h"
#include "JimoCall/JimoCall.h"
#include "Message/MsgId.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoCaller.h"
#include "UtilData/FN.h"


AosDialerManual::AosDialerManual(
		AosRundata *rdata, 
		const AosXmlTagPtr &cfg,
		const u32 dialer_id) 
:
AosJimoCallDialer(rdata, eManual),
mEndPointIdx(0),
mNumTries(eDefaultNumTries)
{
	mJimoCallID = ((u64)dialer_id) << 48;

	bool rslt = config(rdata, cfg);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosDialerManual::~AosDialerManual()
{
}


bool
AosDialerManual::config(AosRundata *rdata, const AosXmlTagPtr &cfg)
{
	aos_assert_r(cfg, false);

	AosXmlTagPtr tag = cfg->getFirstChild();
	while (tag)
	{
		AosEndPointInfo info;
		bool rslt = info.init(rdata, tag);
		aos_assert_r(rslt, false);

		mEndPoints.push_back(info);

		tag = cfg->getNextChild();
	}

	if (mEndPoints.size() <= 0)
	{
		AosLogError(rdata, true, AosErrmsgId::eInvalidConfig)
			<< AosFN::eErrorMsg << "No endpoints configured" << enderr;
		return false;
	}

	OmnString conn_type = cfg->getAttrStr("conn_type", "udp");
	mCubeCommRaw = mCubeComm.getPtr();
	OmnCommListenerPtr thisptr(this, false);
	if (conn_type == "udp")
	{
		mCubeComm = OmnNew AosCubeCommUdp(thisptr);
	}
	else if (conn_type == "tcp")
	{
		mCubeComm = OmnNew AosCubeCommTcpCltNew(thisptr);
	}
	else
	{
		OmnAlarm << "type error" << enderr;
		return false;
	}

	return true;
}


u64
AosDialerManual::nextJimoCallID(AosRundata *rdata)
{
	u64 jimocall_id = __sync_fetch_and_add(&mJimoCallID, 1);
	return jimocall_id;
}


bool
AosDialerManual::makeJimoCall(
		AosRundata *rdata,
		const AosEndPointInfo &endpoint,
		const AosJimoCallPtr &jimo_call,
		AosBuff *buff)
{
	u64 jimocall_id = jimo_call->getJimoCallID();

	mLockRaw->lock();
	mPendingCalls[jimocall_id] = jimo_call;
	mLockRaw->unlock();

	mCubeCommRaw->sendTo(rdata, endpoint, buff);

	return true;
}

/*
bool	
AosDialerManual::getCubeEndpointIDs(
		AosRundata *rdata, 
		vector<int> &endpoint_ids)
{
	// This function retrieves the next mNumTries number of endpoints.
	u32 idx = mEndPointIdx;
	mEndPointIdx += mNumTries;
	if (mEndPointIdx >= mEndPoints.size()) mEndPointIdx %= mEndPoints.size();

	if (idx >= mEndPoints.size()) idx %= mEndPoints.size();

	endpoint_ids.clear();
	for (int i=0; i<mNumTries; i++)
	{
		if (idx >= mEndPoints.size()) idx = 0;
		endpoint_ids.push_back(idx++);
	}

	return true;
}
*/


bool	
AosDialerManual::getTargetCubes(AosRundata *rdata, vector<AosCubeMapObj::CubeInfo> &cube_infos)
{
	OmnNotImplementedYet;
	return false;
}


/*
bool	
AosDialerManual::getCubeEndpointIDs(
		AosRundata *rdata, 
		vector<int> &endpoint_ids)
{
	// This function retrieves the next mNumTries number of endpoints.
	u32 idx = mEndPointIdx;
	mEndPointIdx += mNumTries;
	if (mEndPointIdx >= mEndPoints.size()) mEndPointIdx %= mEndPoints.size();

	if (idx >= mEndPoints.size()) idx %= mEndPoints.size();

	endpoint_ids.clear();
	for (int i=0; i<mNumTries; i++)
	{
		if (idx >= mEndPoints.size()) idx = 0;
		endpoint_ids.push_back(idx++);
	}

	return true;
}
*/


