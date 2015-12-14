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
#include "RlbTester/TesterTrans/RlbTesterSwitchMFinishTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransSvrObj.h"

AosRlbTesterSwitchMFinishTrans::AosRlbTesterSwitchMFinishTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eRlbTesterSwitchMFinish, regflag)
{
	setIsSystemTrans();
}


AosRlbTesterSwitchMFinishTrans::AosRlbTesterSwitchMFinishTrans(
		const int svr_id,
		const u32 proc_id)
:
AosTaskTrans(AosTransType::eRlbTesterSwitchMFinish, 
		svr_id, proc_id, false, false)
{
	setIsSystemTrans();
}


AosRlbTesterSwitchMFinishTrans::~AosRlbTesterSwitchMFinishTrans()
{
}


bool
AosRlbTesterSwitchMFinishTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosRlbTesterSwitchMFinishTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	return true;
}


AosTransPtr
AosRlbTesterSwitchMFinishTrans::clone()
{
	return OmnNew AosRlbTesterSwitchMFinishTrans(false);
}


bool
AosRlbTesterSwitchMFinishTrans::proc()
{
	AosTransSvrObjPtr trans_svr = AosTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	
	bool finish = false;
	bool rslt = trans_svr->isSwitchToMFinish(finish);
	
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}

	AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp->setU8(finish);
	
	return sendResp(resp);
}


