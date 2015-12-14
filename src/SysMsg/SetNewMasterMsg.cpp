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
#include "SysMsg/SetNewMasterMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SEInterfaces/CubeTransSvrObj.h"

AosSetNewMasterMsg::AosSetNewMasterMsg(const bool regflag)
:
AosAppMsg(MsgType::eSetNewMasterMsg, regflag)
{
}


AosSetNewMasterMsg::AosSetNewMasterMsg(
		const int to_svr_id,
		const u32 cube_grp_id,
		const u32 master)
:
AosAppMsg(MsgType::eSetNewMasterMsg, to_svr_id, AosProcessType::eCube, cube_grp_id),
mCubeGrpId(cube_grp_id),
mMaster(master)
{
}


AosSetNewMasterMsg::AosSetNewMasterMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const u32 cube_grp_id,
		const u32 master)
:
AosAppMsg(MsgType::eSetNewMasterMsg, to_svr_id, to_proc_id),
mCubeGrpId(cube_grp_id),
mMaster(master)
{
	aos_assert(to_proc_id == AOSTAG_SVRPROXY_PID); 
}


AosSetNewMasterMsg::~AosSetNewMasterMsg()
{
}


bool
AosSetNewMasterMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCubeGrpId = buff->getU32(0);
	mMaster = buff->getInt(-1);
	return true;
}


bool
AosSetNewMasterMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mCubeGrpId);
	buff->setInt(mMaster);
	return true;
}


AosConnMsgPtr
AosSetNewMasterMsg::clone2()
{
	return OmnNew AosSetNewMasterMsg(false);
}


bool
AosSetNewMasterMsg::proc()
{
	bool rslt;
	if(isToSvrProxy())
	{
		AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
		aos_assert_r(svr_proxy, false);

		rslt = svr_proxy->setNewMaster(mCubeGrpId, mMaster);
	}
	else
	{
		AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
		aos_assert_r(trans_svr, false);
		
		rslt = trans_svr->setNewMaster(mCubeGrpId, mMaster);
	}

	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}
	return true;
}


