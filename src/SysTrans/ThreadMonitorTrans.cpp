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
#include "SysTrans/ThreadMonitorTrans.h"

#include "API/AosApi.h"
#include "SystemMonitor/SysMonitor.h"

AosThreadMonitorTrans::AosThreadMonitorTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eThreadMonitor, regflag)
{
}


AosThreadMonitorTrans::AosThreadMonitorTrans(
		const int svr_id,
		const AosBuffPtr &buff,
//		const vector<OmnString> &errorThrds,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eThreadMonitor, svr_id, need_save, need_resp),
mErrorThrds(buff)
{
}


AosThreadMonitorTrans::~AosThreadMonitorTrans()
{
}


bool
AosThreadMonitorTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	u32 bufflen = buff->getU32(0);
	mErrorThrds = buff->getBuff(bufflen, true AosMemoryCheckerArgs);
/*
	mErrorThrds.clear();
	u32 size = buff->getU32(0);	
	for(u32 i=0; i<size; i++)
	{
		OmnString value;
		buff->getOmnStr(value);
		mErrorThrds.push_back(value);
	}
*/
	return true;
}


bool
AosThreadMonitorTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mErrorThrds->dataLen());
	buff->setBuff(mErrorThrds);
/*
	u32 size = mErrorThrds.size();
	buff->setU32(size);
	
	for(u32 i=0; i<size; i++)
	{
		buff->setOmnStr(mErrorThrds[i]);
	}
*/
	return true;
}


AosTransPtr
AosThreadMonitorTrans::clone()
{
	return OmnNew AosThreadMonitorTrans(false);
}


bool
AosThreadMonitorTrans::proc()
{
	bool rslt = AosSysMonitor::getSelf()->composeLogStr(mErrorThrds);
	aos_assert_r(rslt, false);

	return true;
}


