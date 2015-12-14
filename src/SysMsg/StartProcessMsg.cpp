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
#include "SysMsg/StartProcessMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SvrProxyMgr/ProcessMgr.h"

AosStartProcessMsg::AosStartProcessMsg(const bool regflag)
:
AosAppMsg(MsgType::eStartProcess, regflag)
{
}


AosStartProcessMsg::AosStartProcessMsg(
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eStartProcess, to_svr_id, to_proc_id)
{
}


AosStartProcessMsg::~AosStartProcessMsg()
{
}


bool
AosStartProcessMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStartProcessMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosConnMsgPtr
AosStartProcessMsg::clone2()
{
	return OmnNew AosStartProcessMsg(false);
}


bool
AosStartProcessMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	bool rslt = svr_proxy->startServer();
	aos_assert_r(rslt, false);
	return true;
}


