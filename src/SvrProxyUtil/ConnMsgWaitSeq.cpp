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
#include "SvrProxyUtil/ConnMsgWaitSeq.h"

#include "API/AosApi.h"


AosConnMsgWaitSeq::AosConnMsgWaitSeq(const bool reg_flag)
:
AosConnMsg(MsgType::eWaitSeq, reg_flag)
{
}


AosConnMsgWaitSeq::AosConnMsgWaitSeq(const u32 wait_seq)
:
AosConnMsg(MsgType::eWaitSeq, false),
mWaitSeq(wait_seq)
{
}

AosConnMsgWaitSeq::~AosConnMsgWaitSeq()
{
}


bool
AosConnMsgWaitSeq::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);
	
	buff->setU32(mWaitSeq);
	return true;
}


bool
AosConnMsgWaitSeq::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	mWaitSeq = buff->getU32(0);
	return true;
}


AosConnMsgPtr
AosConnMsgWaitSeq::clone2()
{
	return OmnNew AosConnMsgWaitSeq(false);
}

