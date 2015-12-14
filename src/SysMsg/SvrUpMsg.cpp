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
#include "SysMsg/SvrUpMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosSvrUpMsg::AosSvrUpMsg(const bool reg_flag)
:
AosAppMsg(MsgType::eSvrUpMsg, reg_flag)
{
}


AosSvrUpMsg::AosSvrUpMsg(const int to_svrid, const int up_sid)
:
AosAppMsg(MsgType::eSvrUpMsg, to_svrid, AOSTAG_SVRPROXY_PID),
mUpSvrId(up_sid)
{
}


AosSvrUpMsg::~AosSvrUpMsg()
{
}


bool
AosSvrUpMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mUpSvrId = buff->getInt(-1);
	return true;
}


bool
AosSvrUpMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mUpSvrId);
	return true;
}


AosConnMsgPtr
AosSvrUpMsg::clone2()
{
	return OmnNew AosSvrUpMsg(false);
}


bool
AosSvrUpMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	bool rslt = svr_proxy->recvSvrUp(mUpSvrId);
	aos_assert_r(rslt, false);
	return true;
}

