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
// 04/15/2015	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/NotifyProcIsUp.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosNotifyProcIsUp::AosNotifyProcIsUp(const bool regflag)
:
AosAppMsg(MsgType::eNotifyProcIsUp, regflag)
{
}


AosNotifyProcIsUp::AosNotifyProcIsUp(const int child_pid)
:
AosAppMsg(MsgType::eNotifyProcIsUp, AosGetSelfServerId(), AOSTAG_SVRPROXY_PID),
mChildPid(child_pid)
{
}


AosNotifyProcIsUp::~AosNotifyProcIsUp()
{
}


bool
AosNotifyProcIsUp::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mChildPid = buff->getInt(-1);
	aos_assert_r(mChildPid != -1, false);
	return true;
}


bool
AosNotifyProcIsUp::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mChildPid);
	aos_assert_r(mChildPid != -1, false);
	return true;
}


AosConnMsgPtr
AosNotifyProcIsUp::clone2()
{
	return OmnNew AosNotifyProcIsUp(false);
}

bool
AosNotifyProcIsUp::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	bool rslt = svr_proxy->notifyProcIsUp(mChildPid);
	aos_assert_r(rslt, false);
	return true;
}


