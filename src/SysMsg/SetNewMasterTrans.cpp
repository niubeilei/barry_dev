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

#if 0
#include "SysMsg/SetNewMasterTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransSvrObj.h"

AosSetNewMasterTrans::AosSetNewMasterTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSetNewMaster, regflag)
{
}


AosSetNewMasterTrans::AosSetNewMasterTrans(
		const int svr_id,
		const u32 proc_id,
		const u32 cube_grp_id,
		const int master)
:
AosTaskTrans(AosTransType::eSetNewMaster, svr_id, proc_id, false, true),
mCubeGrpId(cube_grp_id),
mMaster(master)
{
}


AosSetNewMasterTrans::~AosSetNewMasterTrans()
{
}


bool
AosSetNewMasterTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	mMaster = buff->getInt(-1);
	return true;
}


bool
AosSetNewMasterTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId);
	buff->setInt(mMaster);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosSetNewMasterTrans::clone()
{
	return OmnNew AosSetNewMasterTrans(false);
}


bool
AosSetNewMasterTrans::proc()
{
	AosTransSvrObjPtr trans_svr = AosTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);

	bool rslt = trans_svr->setNewMaster(mCubeGrpId, mMaster);	
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}

	AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp->setU8(rslt);

	sendResp(resp);
	return true;
}

#endif
