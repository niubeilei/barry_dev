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
#include "SysMsg/SwitchToBkpTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/CubeTransSvrObj.h"

AosSwitchToBkpTrans::AosSwitchToBkpTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSwitchToBkp, regflag)
{
	setIsSystemTrans(); 
}


AosSwitchToBkpTrans::AosSwitchToBkpTrans(
		const int to_svr_id,
		const u32 cube_grp_id)
:
AosTaskTrans(AosTransType::eSwitchToBkp, to_svr_id,
		AosProcessType::eCube, cube_grp_id, false, true)
{
	setIsSystemTrans(); 
}


AosSwitchToBkpTrans::~AosSwitchToBkpTrans()
{
}


bool
AosSwitchToBkpTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosSwitchToBkpTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


//AosConnMsgPtr
AosTransPtr
AosSwitchToBkpTrans::clone()
{
	return OmnNew AosSwitchToBkpTrans(false);
}


bool
AosSwitchToBkpTrans::proc()
{
	OmnNotImplementedYet;
	return false;
	/*
	AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	
	bool rslt = trans_svr->switchToBkp();
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}

	AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	
	return sendResp(resp);
	*/
}

#endif
