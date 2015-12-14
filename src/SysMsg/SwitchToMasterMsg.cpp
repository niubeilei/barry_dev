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
#include "SysMsg/SwitchToMasterMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SEInterfaces/CubeTransSvrObj.h"

AosSwitchToMasterMsg::AosSwitchToMasterMsg(const bool regflag)
:
AosAppMsg(MsgType::eSwitchToMasterMsg, regflag)
{
}


AosSwitchToMasterMsg::AosSwitchToMasterMsg(
		const int to_svr_id,
		const u32 cube_grp_id)
:
AosAppMsg(MsgType::eSwitchToMasterMsg, to_svr_id, AosProcessType::eCube, cube_grp_id),
mCubeGrpId(cube_grp_id)
{
	// is send to cube.
}


AosSwitchToMasterMsg::AosSwitchToMasterMsg(
		const int to_svr_id,
		const int to_proc_id,
		const u32 cube_grp_id)
:
AosAppMsg(MsgType::eSwitchToMasterMsg, to_svr_id, to_proc_id),
mCubeGrpId(cube_grp_id)
{
	// is send to svrProxy.
	aos_assert(to_proc_id == AOSTAG_SVRPROXY_PID); 
}


AosSwitchToMasterMsg::~AosSwitchToMasterMsg()
{
}


bool
AosSwitchToMasterMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	return true;
}


bool
AosSwitchToMasterMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId);
	return true;
}


AosConnMsgPtr
AosSwitchToMasterMsg::clone2()
{
	return OmnNew AosSwitchToMasterMsg(false);
}


bool
AosSwitchToMasterMsg::proc()
{
	if(isToSvrProxy())
	{
		AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
		aos_assert_r(svr_proxy, false);
		return svr_proxy->switchToMaster(mCubeGrpId);
	}

	// It's to cube svr.
	AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);

	bool rslt = trans_svr->switchToMaster();	
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}
	return true;
}


