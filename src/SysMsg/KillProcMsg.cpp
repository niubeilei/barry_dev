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
#include "SysMsg/KillProcMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosKillProcMsg::AosKillProcMsg(const bool regflag)
:
AosAppMsg(MsgType::eKillProcMsg, regflag)
{
}


AosKillProcMsg::AosKillProcMsg(
		const int to_svr_id,
		const u32 kill_proc_id)
:
AosAppMsg(MsgType::eKillProcMsg, to_svr_id, AOSTAG_SVRPROXY_PID),
mKillProcId(kill_proc_id)
{
}


AosKillProcMsg::~AosKillProcMsg()
{
}


bool
AosKillProcMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mKillProcId = buff->getU32(0);
	return true;
}


bool
AosKillProcMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mKillProcId);
	return true;
}


AosConnMsgPtr
AosKillProcMsg::clone2()
{
	return OmnNew AosKillProcMsg(false);
}


bool
AosKillProcMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	svr_proxy->killProc(mKillProcId);
	return true;
}


