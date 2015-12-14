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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/SvrStoppedMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosSvrStoppedMsg::AosSvrStoppedMsg(const bool regflag)
:
AosAppMsg(MsgType::eSvrStoppedMsg, regflag)
{
}


AosSvrStoppedMsg::AosSvrStoppedMsg(
		const int to_svr_id,
		const int stopped_sid)
:
AosAppMsg(MsgType::eSvrStoppedMsg, to_svr_id, AOSTAG_SVRPROXY_PID),
mStoppedSid(stopped_sid)
{
}


AosSvrStoppedMsg::~AosSvrStoppedMsg()
{
}


bool
AosSvrStoppedMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mStoppedSid = buff->getInt(-1);
	return true;
}


bool
AosSvrStoppedMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mStoppedSid);
	return true;
}


AosConnMsgPtr
AosSvrStoppedMsg::clone2()
{
	return OmnNew AosSvrStoppedMsg(false);
}


bool
AosSvrStoppedMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	svr_proxy->recvSvrStopped(mStoppedSid);
	return true;
}


