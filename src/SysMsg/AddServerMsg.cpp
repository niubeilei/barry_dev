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
#include "SysMsg/AddServerMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "AdminTrans/AdminAddServerTrans.h"

AosAddServerMsg::AosAddServerMsg(const bool regflag)
:
AosAppMsg(MsgType::eAddServer, regflag)
{
}


AosAddServerMsg::AosAddServerMsg(
		const int crt_judger_sid,
		const AosXmlTagPtr &servers_config,
		const AosXmlTagPtr &admin_config,
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eAddServer, to_svr_id, to_proc_id),
mCrtJudgerSid(crt_judger_sid),
mServersConfig(servers_config),
mAdminConfig(admin_config)
{
}


AosAddServerMsg::AosAddServerMsg(
		const AosXmlTagPtr &servers_config,
		const AosXmlTagPtr &admin_config,
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eAddServer, to_svr_id, to_proc_id),
mCrtJudgerSid(-1),
mServersConfig(servers_config),
mAdminConfig(admin_config)
{
}


AosAddServerMsg::~AosAddServerMsg()
{
}


bool
AosAddServerMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCrtJudgerSid = buff->getInt(-1);
	
	AosXmlParser parser;
	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mServersConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mServersConfig, false);

	str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mAdminConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mAdminConfig, false);

	return true;
}


bool
AosAddServerMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mCrtJudgerSid);
	aos_assert_r(mServersConfig, false);
	buff->setOmnStr(mServersConfig->toString());

	aos_assert_r(mAdminConfig, false);
	buff->setOmnStr(mAdminConfig->toString());

	return true;
}


AosConnMsgPtr
AosAddServerMsg::clone2()
{
	return OmnNew AosAddServerMsg(false);
}


bool
AosAddServerMsg::proc()
{

	aos_assert_r(mServersConfig && mAdminConfig, false);
	bool rslt = false;

	if(isToSvrProxy())
	{
		AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
		aos_assert_r(svr_proxy, false);
		rslt = svr_proxy->addServers(mCrtJudgerSid, mServersConfig, mAdminConfig);
		aos_assert_r(rslt, false);
		return true;
	}

	aos_assert_r(getToProcId() == AOSTAG_ADMIN_PID, false);
	
	AosNetworkMgrObjPtr network_mgr = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(network_mgr, false);
	
	rslt = network_mgr->configSelfSvr(mAdminConfig);
	aos_assert_r(rslt, false);
	
	rslt = network_mgr->configServers(mAdminConfig);
	aos_assert_r(rslt, false);
	
	return true;
}


