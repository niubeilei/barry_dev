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
#include "SysTrans/TellServerDeathTrans.h"

#include "API/AosApi.h"
#include "SystemMonitor/SysMonitor.h"

AosTellServerDeathTrans::AosTellServerDeathTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eTellServerDeath, regflag)
{
}


AosTellServerDeathTrans::AosTellServerDeathTrans(
		const int deathid,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eTellServerDeath, svr_id, need_save, need_resp),
mDeathId(deathid)
{
}


AosTellServerDeathTrans::~AosTellServerDeathTrans()
{
}


bool
AosTellServerDeathTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDeathId = buff->getInt(0);
	return true;
}


bool
AosTellServerDeathTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mDeathId);
	return true;
}


AosTransPtr
AosTellServerDeathTrans::clone()
{
	return OmnNew AosTellServerDeathTrans(false);
}


bool
AosTellServerDeathTrans::proc()
{
	AosSysMonitor::getSelf()->setServerDeath(mDeathId);

	return true;
}


