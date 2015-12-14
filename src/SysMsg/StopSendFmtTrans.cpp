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
#include "SysMsg/StopSendFmtTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/CubeTransSvrObj.h"

AosStopSendFmtTrans::AosStopSendFmtTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eStopSendFmt, regflag)
{
	setIsSystemTrans(); 
}


AosStopSendFmtTrans::AosStopSendFmtTrans(
		const int to_svr_id,
		const u32 cube_grp_id)
:
AosTaskTrans(AosTransType::eStopSendFmt, to_svr_id,
		AosProcessType::eCube, cube_grp_id, false, true)
{
	setIsSystemTrans(); 
}


AosStopSendFmtTrans::~AosStopSendFmtTrans()
{
}


bool
AosStopSendFmtTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosStopSendFmtTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


//AosConnMsgPtr
AosTransPtr
AosStopSendFmtTrans::clone()
{
	return OmnNew AosStopSendFmtTrans(false);
}


bool
AosStopSendFmtTrans::proc()
{
	AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	
	bool rslt = trans_svr->stopSendFmt();
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}

	AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	
	return sendResp(resp);
}


