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
// Created: 05/08/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/TransSvrDeathMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransCltObj.h"

AosTransSvrDeathMsg::AosTransSvrDeathMsg(const bool reg_flag)
:
AosAppMsg(MsgType::eTransSvrDeathMsg, reg_flag)
{
}


AosTransSvrDeathMsg::AosTransSvrDeathMsg(
		const int to_svrid,
		const u32 to_proc_id,
		const AosTransId &req_id,
		const int death_svr_id)
:
AosAppMsg(MsgType::eTransSvrDeathMsg, to_svrid, to_proc_id),
mReqId(req_id),
mDeathSvrId(death_svr_id)
{
}



AosTransSvrDeathMsg::~AosTransSvrDeathMsg()
{
}


bool
AosTransSvrDeathMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	mReqId = AosTransId::serializeFrom(buff);
	mDeathSvrId = buff->getInt(-1);
	return true;
}


bool
AosTransSvrDeathMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);

	mReqId.serializeTo(buff);
	buff->setInt(mDeathSvrId);
	return true;
}


AosConnMsgPtr
AosTransSvrDeathMsg::clone2()
{
	return OmnNew AosTransSvrDeathMsg(false);
}


bool
AosTransSvrDeathMsg::proc()
{
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);

	return trans_clt->recvSvrDeath(mReqId, mDeathSvrId);
}


