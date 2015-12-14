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
#include "SysMsg/AddClusterMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SvrProxyMgr/ProcessMgr.h"

AosAddClusterMsg::AosAddClusterMsg(const bool regflag)
:
AosAppMsg(MsgType::eAddCluster, regflag)
{
}


AosAddClusterMsg::AosAddClusterMsg(
		const OmnString &args,
		const AosXmlTagPtr &cluster_config,
		const AosXmlTagPtr &norm_config,
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eAddCluster, to_svr_id, to_proc_id),
mArgs(args),
mClusterConfig(cluster_config),
mNormConfig(norm_config)
{
}


AosAddClusterMsg::~AosAddClusterMsg()
{
}


bool
AosAddClusterMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mArgs = buff->getOmnStr("");
	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	AosXmlParser parser;
	mClusterConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mClusterConfig, false);
	str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mNormConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(mNormConfig, false);

	return true;
}


bool
AosAddClusterMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mArgs);
	aos_assert_r(mClusterConfig, false);
	buff->setOmnStr(mClusterConfig->toString());
	aos_assert_r(mNormConfig, false);
	buff->setOmnStr(mNormConfig->toString());

	return true;
}


AosConnMsgPtr
AosAddClusterMsg::clone2()
{
	return OmnNew AosAddClusterMsg(false);
}


bool
AosAddClusterMsg::proc()
{
	aos_assert_r(mClusterConfig && mNormConfig, false);
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);

	bool rslt = svr_proxy->addCluster(
			getFromProcId(), 
			mArgs, mClusterConfig, mNormConfig);
	aos_assert_r(rslt, false);
	return true;
}


