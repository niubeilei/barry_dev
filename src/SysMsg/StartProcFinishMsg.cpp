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
#include "SysMsg/StartProcFinishMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "SEInterfaces/MsgClientObj.h"
#include "SEInterfaces/ServiceMgrObj.h"

AosStartProcFinishMsg::AosStartProcFinishMsg(const bool regflag)
:
AosAppMsg(MsgType::eStartProcFinish, regflag)
{
}


AosStartProcFinishMsg::AosStartProcFinishMsg(
		const u32 to_proc_id,
		const AosProcessType::E proc_tp,
		const u32 start_proc_id,
		const int start_proc_pid,
		const int to_svr_id)
:
AosAppMsg(MsgType::eStartProcFinish, to_svr_id, to_proc_id),
mProcType(proc_tp),
mStartProcId(start_proc_id),
mStartProcPid(start_proc_pid)
{
}




AosStartProcFinishMsg::AosStartProcFinishMsg(
		const u32 to_proc_id,
		const AosProcessType::E proc_tp,
		const u32 start_proc_id,
		const int start_proc_pid)
:
AosAppMsg(MsgType::eStartProcFinish, AosGetSelfServerId(), to_proc_id),
mProcType(proc_tp),
mStartProcId(start_proc_id),
mStartProcPid(start_proc_pid)
{
}


AosStartProcFinishMsg::~AosStartProcFinishMsg()
{
}


bool
AosStartProcFinishMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mProcType = (AosProcessType::E)buff->getU32(0);
	mStartProcId = buff->getU32(0);
	aos_assert_r(mStartProcId != 0, false);
	mStartProcPid = buff->getInt(-1);
	aos_assert_r(mStartProcPid != -1, false);
	return true;
}


bool
AosStartProcFinishMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU32(mProcType);
	aos_assert_r(mStartProcId != 0, false);
	buff->setU32(mStartProcId);
	aos_assert_r(mStartProcPid != -1, false);
	buff->setInt(mStartProcPid);
	return true;
}


AosConnMsgPtr
AosStartProcFinishMsg::clone2()
{
	return OmnNew AosStartProcFinishMsg(false);
}

bool
AosStartProcFinishMsg::proc()
{
	if (mProcType == AosProcessType::eTask)
	{
		return AosTaskMgrObj::startTaskProcCbStatic(mStartProcId, mStartProcPid, getFromSvrId());
	}
	else if (mProcType == AosProcessType::eService)
	{
		return AosServiceMgrObj::startTaskProcCbStatic(mStartProcId, mStartProcPid, getFromSvrId());
	}
	else if (mProcType == AosProcessType::eMsg)
	{
		return AosMsgClientObj::startProcCbStatic(mStartProcId);
	}
	else
	{
		OmnNotImplementedYet;
	}
	return true;
}


