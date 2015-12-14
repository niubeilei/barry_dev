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
#include "SysMsg/ReSwitchToMasterMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SEInterfaces/CubeTransSvrObj.h"

AosReSwitchToMasterMsg::AosReSwitchToMasterMsg(const bool regflag)
:
AosAppMsg(MsgType::eReSwitchToMasterMsg, regflag)
{
}


AosReSwitchToMasterMsg::AosReSwitchToMasterMsg(
		const int to_svr_id,
		const u32 cube_grp_id)
:
AosAppMsg(MsgType::eReSwitchToMasterMsg, to_svr_id, AosProcessType::eCube, cube_grp_id),
mCubeGrpId(cube_grp_id)
{
}


AosReSwitchToMasterMsg::~AosReSwitchToMasterMsg()
{
}


bool
AosReSwitchToMasterMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	return true;
}


bool
AosReSwitchToMasterMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId);
	return true;
}


AosConnMsgPtr
AosReSwitchToMasterMsg::clone2()
{
	return OmnNew AosReSwitchToMasterMsg(false);
}


bool
AosReSwitchToMasterMsg::proc()
{
	// It's to cube svr.
	AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);

	bool rslt = trans_svr->reSwitchToMaster();	
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}
	return true;
}


