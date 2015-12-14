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
#include "TransUtil/TransAckMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransCltObj.h"

AosTransAckMsg::AosTransAckMsg(const bool reg_flag)
:
AosAppMsg(MsgType::eTransAckMsg, reg_flag)
{
}


AosTransAckMsg::AosTransAckMsg(
		const int to_svrid,
		const u32 to_proc_id,
		const AosTransId &req_id)
:
AosAppMsg(MsgType::eTransAckMsg, to_svrid, to_proc_id),
mReqId(req_id)
{
}



AosTransAckMsg::~AosTransAckMsg()
{
}


bool
AosTransAckMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);
	
	mReqId = AosTransId::serializeFrom(buff);
	return true;
}


bool
AosTransAckMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);
	
	mReqId.serializeTo(buff);
	return true;
}


AosConnMsgPtr
AosTransAckMsg::clone2()
{
	return OmnNew AosTransAckMsg(false);
}


bool
AosTransAckMsg::proc()
{
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);

	return trans_clt->recvAck(mReqId, getFromSvrId());
}


