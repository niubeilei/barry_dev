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
#include "SysMsg/SetProcStatusMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SvrProxyMgr/ProcessMgr.h"

#if 0
AosSetProcStatusMsg::AosSetProcStatusMsg(const bool regflag)
:
AosAppMsg(MsgType::eSetProcStatus, regflag)
{
}


AosSetProcStatusMsg::AosSetProcStatusMsg(
		const AosProcInfo::Status status,
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eSetProcStatus, to_svr_id, to_proc_id),
mStatus(status)
{
}


AosSetProcStatusMsg::~AosSetProcStatusMsg()
{
}


bool
AosSetProcStatusMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mStatus = (AosProcInfo::Status)(buff->getU32(0));
	return true;
}


bool
AosSetProcStatusMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU32(mStatus);
	return true;
}


AosConnMsgPtr
AosSetProcStatusMsg::clone2()
{
	return OmnNew AosSetProcStatusMsg(false);
}


bool
AosSetProcStatusMsg::proc()
{
	AosNetworkMgrObjPtr network_mgr = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(network_mgr, false);
	bool rslt = network_mgr->setStatus(getFromSvrId(), getFromProcId(), mStatus);
	aos_assert_r(rslt, false);
	return true;
}

#endif
