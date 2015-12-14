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
#include "SvrProxyUtil/ConnMsgGetWaitSeq.h"

#include "API/AosApi.h"


AosConnMsgGetWaitSeq::AosConnMsgGetWaitSeq(const bool reg_flag)
:
AosConnMsg(MsgType::eGetWaitSeq, reg_flag)
{
}


AosConnMsgGetWaitSeq::AosConnMsgGetWaitSeq()
:
AosConnMsg(MsgType::eGetWaitSeq, false)
{
}

AosConnMsgGetWaitSeq::~AosConnMsgGetWaitSeq()
{
}


bool
AosConnMsgGetWaitSeq::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosConnMsgGetWaitSeq::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	return true;
}


AosConnMsgPtr
AosConnMsgGetWaitSeq::clone2()
{
	return OmnNew AosConnMsgGetWaitSeq(false);
}

