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
#include "SysMsg/ResendEndMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosResendEndMsg::AosResendEndMsg(const bool regflag)
:
AosAppMsg(MsgType::eResendEndMsg, regflag)
{
}


AosResendEndMsg::AosResendEndMsg(
		const int target_sid, 
		const u32 target_pid,
		const bool is_ipc_conn,
		const int from_sid,
		const u32 from_pid)
:
AosAppMsg(MsgType::eResendEndMsg, target_sid, AOSTAG_SVRPROXY_PID),
mTargetSid(target_sid),
mTargetPid(target_pid),
mIsIpcConn(is_ipc_conn),
mFromSid(from_sid),
mFromPid(from_pid)
{
}


AosResendEndMsg::~AosResendEndMsg()
{
}


bool
AosResendEndMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mTargetSid = buff->getInt(-1);
	mTargetPid = buff->getU32(0);
	mIsIpcConn = buff->getU8(0);
	mFromSid = buff->getInt(-1);
	mFromPid = buff->getU32(0);
	return true;
}


bool
AosResendEndMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mTargetSid);
	buff->setU32(mTargetPid);
	buff->setU8(mIsIpcConn);
	buff->setInt(mFromSid);
	buff->setU32(mFromPid);
	return true;
}


AosConnMsgPtr
AosResendEndMsg::clone2()
{
	return OmnNew AosResendEndMsg(false);
}


bool
AosResendEndMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	bool rslt = svr_proxy->recvResendEnd(mTargetSid, mTargetPid,
			mIsIpcConn, mFromSid, mFromPid);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}
	return true;
}


