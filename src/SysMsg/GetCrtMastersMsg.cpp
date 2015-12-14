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
#include "SysMsg/GetCrtMastersMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosGetCrtMastersMsg::AosGetCrtMastersMsg(const bool regflag)
:
AosAppMsg(MsgType::eGetCrtMastersMsg, regflag)
{
}


AosGetCrtMastersMsg::AosGetCrtMastersMsg(const int to_svr_id)
:
AosAppMsg(MsgType::eGetCrtMastersMsg, to_svr_id, AOSTAG_SVRPROXY_PID)
{
}

AosGetCrtMastersMsg::~AosGetCrtMastersMsg()
{
}


bool
AosGetCrtMastersMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosGetCrtMastersMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


AosConnMsgPtr
AosGetCrtMastersMsg::clone2()
{
	return OmnNew AosGetCrtMastersMsg(false);
}


bool
AosGetCrtMastersMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);

	svr_proxy->getCrtMastersBuff(getFromSvrId());
	return true;
}


