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
#include "SvrProxyUtil/ConnMsgAck.h"

#include "API/AosApi.h"


AosConnMsgAck::AosConnMsgAck(const bool reg_flag)
:
AosConnMsg(MsgType::eConnAck, reg_flag)
{
}


AosConnMsgAck::AosConnMsgAck(const u32 ack_seq)
:
AosConnMsg(MsgType::eConnAck, false),
mAckSeq(ack_seq)
{
}

AosConnMsgAck::~AosConnMsgAck()
{
}


bool
AosConnMsgAck::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);
	
	buff->setU32(mAckSeq);
	return true;
}


bool
AosConnMsgAck::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	mAckSeq = buff->getU32(0);
	return true;
}


AosConnMsgPtr
AosConnMsgAck::clone2()
{
	return OmnNew AosConnMsgAck(false);
}

