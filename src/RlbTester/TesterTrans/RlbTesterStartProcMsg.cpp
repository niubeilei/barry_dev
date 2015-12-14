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
#include "RlbTester/TesterTrans/RlbTesterStartProcMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosRlbTesterStartProcMsg::AosRlbTesterStartProcMsg(const bool regflag)
:
AosAppMsg(MsgType::eRlbTesterStartProcMsg, regflag)
{
}


AosRlbTesterStartProcMsg::AosRlbTesterStartProcMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const u32 start_proc_id)
:
AosAppMsg(MsgType::eRlbTesterStartProcMsg, to_svr_id, to_proc_id),
mStartProcId(start_proc_id)
{
}


AosRlbTesterStartProcMsg::~AosRlbTesterStartProcMsg()
{
}


bool
AosRlbTesterStartProcMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mStartProcId = buff->getU32(0);
	return true;
}


bool
AosRlbTesterStartProcMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mStartProcId);
	return true;
}


AosConnMsgPtr
AosRlbTesterStartProcMsg::clone2()
{
	return OmnNew AosRlbTesterStartProcMsg(false);
}


bool
AosRlbTesterStartProcMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	svr_proxy->startProc(mStartProcId);
	return true;
}


