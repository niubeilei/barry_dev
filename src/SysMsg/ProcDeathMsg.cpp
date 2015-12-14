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
#include "SysMsg/ProcDeathMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "SEInterfaces/ServiceMgrObj.h"

AosProcDeathMsg::AosProcDeathMsg(const bool regflag)
:
AosAppMsg(MsgType::eProcDeathMsg, regflag)
{
}


AosProcDeathMsg::AosProcDeathMsg(
		const AosProcessType::E proc_type,
		const int to_svr_id,
		const u32 to_proc_id,
		const int death_sid,
		const u32 death_pid)
:
AosAppMsg(MsgType::eProcDeathMsg, to_svr_id, to_proc_id),
mDeathSvrId(death_sid),
mDeathProcId(death_pid),
mProcType(proc_type)
{
}


AosProcDeathMsg::~AosProcDeathMsg()
{
}


bool
AosProcDeathMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDeathSvrId = buff->getInt(0);
	mDeathProcId = buff->getU32(0);
	mProcType = AosProcessType::toEnum(buff->getOmnStr(""));
	aos_assert_r(AosProcessType::isValid(mProcType), false);
	return true;
}


bool
AosProcDeathMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mDeathSvrId);
	buff->setU32(mDeathProcId);
	aos_assert_r(AosProcessType::isValid(mProcType), false);
	buff->setOmnStr(AosProcessType::toStr(mProcType));
	return true;
}


AosConnMsgPtr
AosProcDeathMsg::clone2()
{
	return OmnNew AosProcDeathMsg(false);
}


bool
AosProcDeathMsg::proc()
{
	bool rslt = false;
	if (mProcType == AosProcessType::eTask)
	{
		rslt = AosTaskMgrObj::stopTaskProcCbStatic(mDeathProcId, mDeathSvrId);
	}
	else if (mProcType == AosProcessType::eService)
	{
		rslt = AosServiceMgrObj::stopTaskProcCbStatic(mDeathProcId, mDeathSvrId);
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}
	aos_assert_r(rslt, false);
	
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}
	return true;
}


