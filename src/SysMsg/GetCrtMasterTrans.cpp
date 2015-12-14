////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 05/08/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0

#include "SysMsg/GetCrtMasterTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosGetCrtMasterTrans::AosGetCrtMasterTrans(const bool reg_flag)
:
AosTaskTrans(AosTransType::eGetCrtMaster, reg_flag)
{
	setIsSystemTrans();
}


AosGetCrtMasterTrans::AosGetCrtMasterTrans(
		const int to_svrid,
		const u32 to_proc_id,
		const u32 cube_grp_id)
:
AosTaskTrans(AosTransType::eGetCrtMaster, to_svrid, to_proc_id,
		false, true),
mCubeGrpId(cube_grp_id)
{
	setIsSystemTrans();
}


AosGetCrtMasterTrans::~AosGetCrtMasterTrans()
{
}


bool
AosGetCrtMasterTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCubeGrpId = buff->getU32(0);
	return true;
}


bool
AosGetCrtMasterTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mCubeGrpId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosGetCrtMasterTrans::clone()
{
	return OmnNew AosGetCrtMasterTrans(false);
}


bool
AosGetCrtMasterTrans::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	AosTransPtr thisptr(this, false);
	svr_proxy->getCrtMaster(thisptr, mCubeGrpId);

	//int master = svr_proxy->getCrtMaster(mCubeGrpId);
	//AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	//resp->setInt(master);

	//sendResp(resp);
	return true;
}

#endif
