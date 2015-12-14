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
#include "SysMsg/StartDynamicProcMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosStartDynamicProcMsg::AosStartDynamicProcMsg(const bool regflag)
:
AosAppMsg(MsgType::eStartDynamicProc, regflag)
{
}


AosStartDynamicProcMsg::AosStartDynamicProcMsg(const AosProcessType::E proc_tp) 
:
AosAppMsg(MsgType::eStartDynamicProc, AosGetSelfServerId(), AOSTAG_SVRPROXY_PID),
mProcType(proc_tp),
mListenSvrId(AosGetSelfServerId())
{
}


AosStartDynamicProcMsg::AosStartDynamicProcMsg(
					const AosProcessType::E proc_tp, 
					const int listen_svr_id,
					const int to_svr_id)
:
AosAppMsg(MsgType::eStartDynamicProc, to_svr_id, AOSTAG_SVRPROXY_PID),
mProcType(proc_tp),
mListenSvrId(listen_svr_id)
{
}


AosStartDynamicProcMsg::~AosStartDynamicProcMsg()
{
}


bool
AosStartDynamicProcMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mProcType = (AosProcessType::E)buff->getU32(0);
	mListenSvrId = buff->getInt(-1);
	aos_assert_r(mListenSvrId >= 0, false);
	return true;
}


bool
AosStartDynamicProcMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU32(mProcType);
	aos_assert_r(mListenSvrId >= 0, false);
	buff->setInt(mListenSvrId);
	return true;
}


AosConnMsgPtr
AosStartDynamicProcMsg::clone2()
{
	return OmnNew AosStartDynamicProcMsg(false);
}

bool
AosStartDynamicProcMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
OmnScreen << "StartDynamicProcMsg"
		  << " , listen_svr_id: " << mListenSvrId
		  << " , listen_proc_id: " << getFromProcId()
		  << endl;
	svr_proxy->startProc(getFromProcId(), mProcType, mListenSvrId);
	return true;
}


