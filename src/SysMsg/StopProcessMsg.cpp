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
#include "SysMsg/StopProcessMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SvrProxyMgr/ProcessMgr.h"

AosStopProcessMsg::AosStopProcessMsg(const bool regflag)
:
AosAppMsg(MsgType::eStopProcess, regflag)
{
}


AosStopProcessMsg::AosStopProcessMsg(
		const int signal_no,
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eStopProcess, to_svr_id, to_proc_id),
mSignalNo(signal_no)
{
}


AosStopProcessMsg::~AosStopProcessMsg()
{
}


bool
AosStopProcessMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mSignalNo = buff->getInt(14);
	return true;
}


bool
AosStopProcessMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setInt(mSignalNo);
	return true;
}


AosConnMsgPtr
AosStopProcessMsg::clone2()
{
	return OmnNew AosStopProcessMsg(false);
}


bool
AosStopProcessMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);

	bool rslt = svr_proxy->stopProcExcludeAdmin(mSignalNo);
	aos_assert_r(rslt, false);
	return true;
}


