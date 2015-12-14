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
#include "SysMsg/SwitchToMasterFinishMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosSwitchToMasterFinishMsg::AosSwitchToMasterFinishMsg(const bool regflag)
:
AosAppMsg(MsgType::eSwitchToMasterFinishMsg, regflag)
{
}


AosSwitchToMasterFinishMsg::AosSwitchToMasterFinishMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const u32 finish_proc_id,
		const u32 cube_grp_id)
:
AosAppMsg(MsgType::eSwitchToMasterFinishMsg, to_svr_id, to_proc_id),
mFinishProcId(finish_proc_id),
mCubeGrpId(cube_grp_id)
{
}


AosSwitchToMasterFinishMsg::~AosSwitchToMasterFinishMsg()
{
}


bool
AosSwitchToMasterFinishMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	mFinishProcId = buff->getU32(0);
	return true;
}


bool
AosSwitchToMasterFinishMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId);
	buff->setU32(mFinishProcId);
	return true;
}


AosConnMsgPtr
AosSwitchToMasterFinishMsg::clone2()
{
	return OmnNew AosSwitchToMasterFinishMsg(false);
}


bool
AosSwitchToMasterFinishMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	return svr_proxy->switchToMasterFinish(mCubeGrpId, mFinishProcId);
}


