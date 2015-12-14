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
#include "SysMsg/TriggerResendMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/SvrProxyObj.h"

AosTriggerResendMsg::AosTriggerResendMsg(const bool regflag)
:
AosAppMsg(MsgType::eTriggerResendMsg, regflag)
{
}


AosTriggerResendMsg::AosTriggerResendMsg(const OmnString reason)
:
AosAppMsg(MsgType::eTriggerResendMsg, AosGetSelfServerId(), AOSTAG_SVRPROXY_PID),
mReason(reason),
mNeedResendEnd(false),
mTargetSvrId(-1),
mTargetProcId(0),
mIsIpcConn(false)
{
}

AosTriggerResendMsg::AosTriggerResendMsg(
		const OmnString &reason, 
		const int target_svr_id,
		const u32 target_proc_id,
		const bool is_ipc_conn)
:
AosAppMsg(MsgType::eTriggerResendMsg, AosGetSelfServerId(), AOSTAG_SVRPROXY_PID),
mReason(reason),
mNeedResendEnd(true),
mTargetSvrId(target_svr_id),
mTargetProcId(target_proc_id),
mIsIpcConn(is_ipc_conn)
{
}

AosTriggerResendMsg::~AosTriggerResendMsg()
{
}


bool
AosTriggerResendMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mReason = buff->getOmnStr("");
	mNeedResendEnd = buff->getU8(0);
	mTargetSvrId = buff->getInt(-1);
	mTargetProcId = buff->getU32(0);
	mIsIpcConn = buff->getU8(0);
	return true;
}


bool
AosTriggerResendMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mReason);
	buff->setU8(mNeedResendEnd);
	buff->setInt(mTargetSvrId);
	buff->setU32(mTargetProcId);
	buff->setU8(mIsIpcConn);
	return true;
}


AosConnMsgPtr
AosTriggerResendMsg::clone2()
{
	return OmnNew AosTriggerResendMsg(false);
}


bool
AosTriggerResendMsg::proc()
{
	bool rslt = false;
	AosTriggerResendMsgPtr thisptr(this, false);
	if(isToSvrProxy())
	{
		AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
		aos_assert_r(svr_proxy, false);

		rslt = svr_proxy->triggerResend(thisptr);
		aos_assert_r(rslt, false);
		return true;	
	}
	
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	rslt = trans_clt->triggerResend(thisptr);
	aos_assert_r(rslt, false);
	return true;
}


