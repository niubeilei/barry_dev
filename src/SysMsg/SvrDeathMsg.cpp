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
#include "SysMsg/SvrDeathMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosSvrDeathMsg::AosSvrDeathMsg(const bool reg_flag)
:
AosAppMsg(MsgType::eSvrDeathMsg, reg_flag)
{
}


AosSvrDeathMsg::AosSvrDeathMsg(const int to_svrid, const int death_sid)
:
AosAppMsg(MsgType::eSvrDeathMsg, to_svrid, AOSTAG_SVRPROXY_PID),
mDeathSvrId(death_sid)
{
}


AosSvrDeathMsg::~AosSvrDeathMsg()
{
}


bool
AosSvrDeathMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDeathSvrId = buff->getInt(-1);
	return true;
}


bool
AosSvrDeathMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mDeathSvrId);
	return true;
}


AosConnMsgPtr
AosSvrDeathMsg::clone2()
{
	return OmnNew AosSvrDeathMsg(false);
}


bool
AosSvrDeathMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	bool rslt = svr_proxy->recvSvrDeath(mDeathSvrId);
	aos_assert_r(rslt, false);
	return true;
}

