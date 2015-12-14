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
#include "SysMsg/ProcUpMsg.h"

#include "API/AosApi.h"

AosProcUpMsg::AosProcUpMsg(const bool regflag)
:
AosAppMsg(MsgType::eProcUpMsg, regflag)
{
}


AosProcUpMsg::AosProcUpMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const int up_sid,
		const u32 up_pid)
:
AosAppMsg(MsgType::eProcUpMsg, to_svr_id, to_proc_id),
mUpSvrId(up_sid),
mUpProcId(up_pid)
{
}


AosProcUpMsg::~AosProcUpMsg()
{
}


bool
AosProcUpMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mUpSvrId = buff->getInt(0);
	mUpProcId = buff->getU32(0);
	return true;
}


bool
AosProcUpMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mUpSvrId);
	buff->setU32(mUpProcId);
	return true;
}


AosConnMsgPtr
AosProcUpMsg::clone2()
{
	return OmnNew AosProcUpMsg(false);
}


bool
AosProcUpMsg::proc()
{
	OmnAlarm << "error!" << enderr;
	return true;
}


