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
#include "SysMsg/CheckStartJobSvrMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosCheckStartJobSvrMsg::AosCheckStartJobSvrMsg(const bool regflag)
:
AosAppMsg(MsgType::eCheckStartJobSvrMsg, regflag)
{
}


AosCheckStartJobSvrMsg::AosCheckStartJobSvrMsg()
:
AosAppMsg(MsgType::eCheckStartJobSvrMsg, AosGetSelfServerId(), AOSTAG_SVRPROXY_PID)
{
}


AosCheckStartJobSvrMsg::~AosCheckStartJobSvrMsg()
{
}


bool
AosCheckStartJobSvrMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCheckStartJobSvrMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosConnMsgPtr
AosCheckStartJobSvrMsg::clone2()
{
	return OmnNew AosCheckStartJobSvrMsg(false);
}

bool
AosCheckStartJobSvrMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	if (svr_proxy->selfIsJudgerSvr())
	{
		bool rslt = svr_proxy->startJobSvr();
		aos_assert_r(rslt, false);
	}
	return true;
}


