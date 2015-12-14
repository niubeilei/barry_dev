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
#include "SysMsg/GetClusterConfigMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosGetClusterConfigMsg::AosGetClusterConfigMsg(const bool regflag)
:
AosAppMsg(MsgType::eGetClusterConfigMsg, regflag)
{
}


AosGetClusterConfigMsg::AosGetClusterConfigMsg(const int to_svr_id)
:
AosAppMsg(MsgType::eGetClusterConfigMsg, to_svr_id, AOSTAG_SVRPROXY_PID)
{
}

AosGetClusterConfigMsg::~AosGetClusterConfigMsg()
{
}


bool
AosGetClusterConfigMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosGetClusterConfigMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


AosConnMsgPtr
AosGetClusterConfigMsg::clone2()
{
	return OmnNew AosGetClusterConfigMsg(false);
}


bool
AosGetClusterConfigMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);

	svr_proxy->getCluster(getFromSvrId());
	return true;
}


