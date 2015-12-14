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
// 05/17/2013	Created by Young Pan 
////////////////////////////////////////////////////////////////////////////
#include "SysTrans/AskWheatherDeathTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SystemMonitor/SysMonitor.h"

AosAskWheatherDeathTrans::AosAskWheatherDeathTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAskWheatherDeath, regflag)
{

}


AosAskWheatherDeathTrans::AosAskWheatherDeathTrans(
		const u32 start_time,
		const int deathid,
		const int svr_id,
		const bool need_save,	
		const bool need_resp)
:
AosTaskTrans(AosTransType::eAskWheatherDeath, svr_id, need_save, need_resp),
mStartTime(start_time),
mDeathId(deathid),
mServerId(svr_id)
{
}


AosAskWheatherDeathTrans::~AosAskWheatherDeathTrans()
{
}


bool
AosAskWheatherDeathTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	//rslt = AosAsyncReqTrans::serializeFrom(buff);
	//aos_assert_r(rslt, false);

	mStartTime = buff->getU32(0);
	mDeathId = buff->getInt(0);
	mServerId = buff->getInt(0);
	return true;
}


bool
AosAskWheatherDeathTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	//rslt = AosAsyncReqTrans::serializeTo(buff);
	//aos_assert_r(rslt, false);

	buff->setU32(mStartTime);
	buff->setInt(mDeathId);
	buff->setInt(mServerId);
	return true;
}


AosTransPtr
AosAskWheatherDeathTrans::clone()
{
	return OmnNew AosAskWheatherDeathTrans(false);
}


bool
AosAskWheatherDeathTrans::proc()
{
	int buffsize = sizeof(int) *2 + sizeof(u32) + sizeof(u8) + 10;
	
	bool death = AosSysMonitor::getSelf()->getStatus(mDeathId);

	AosBuffPtr buff = OmnNew AosBuff(buffsize AosMemoryCheckerArgs);
	buff->setU32(mStartTime);
	buff->setInt(mDeathId);
	buff->setInt(mServerId);
	buff->setU8(death);

	sendResp(buff);
	return true;
}


bool
AosAskWheatherDeathTrans::respCallBack()
{
	// Ketty 2013/07/20
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();
	AosTransPtr thisptr(this, false);
	
	AosSysMonitor::getSelf()->askDeathCallback(
			thisptr, resp, svr_death);
	return true;
}


