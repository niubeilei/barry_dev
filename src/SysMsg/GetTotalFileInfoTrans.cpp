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
#include "SysMsg/GetTotalFileInfoTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/VfsMgrObj.h"

AosGetTotalFileInfoTrans::AosGetTotalFileInfoTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetTotalFileInfo, regflag)
{
	setIsSystemTrans();
}


AosGetTotalFileInfoTrans::AosGetTotalFileInfoTrans(
		const int svr_id,
		const u32 cube_grp_id,
		const u32 cube_id)
:
AosTaskTrans(AosTransType::eGetTotalFileInfo, svr_id,
		AosProcessType::eCube, cube_grp_id, false, true),
mCubeId(cube_id)
{
	setIsSystemTrans();
}


AosGetTotalFileInfoTrans::~AosGetTotalFileInfoTrans()
{
}


bool
AosGetTotalFileInfoTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeId = buff->getU32(0);
	return true;
}


bool
AosGetTotalFileInfoTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosGetTotalFileInfoTrans::clone()
{
	return OmnNew AosGetTotalFileInfoTrans(false);
}


bool
AosGetTotalFileInfoTrans::proc()
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	
	AosBuffPtr data_buff;
	bool rslt = vfs_mgr->getTotalFileInfo(mRdata.getPtrNoLock(), mCubeId, data_buff);

	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setBuff(data_buff);

	sendResp(resp_buff);
	return true;
}


