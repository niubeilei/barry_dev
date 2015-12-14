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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransServer/ProcThrdMonitor.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "TransServer/TransSvrThrd.h"

AosProcThrdMonitor::AosProcThrdMonitor(
		const u32 thrd_num, 
		AosTransSvrThrdPtr * thrds)
:
mThrdNum(thrd_num),
mProcThrds(thrds)
{
	OmnThreadedObjPtr thisptr(this, false);
	mMonitorThrd = OmnNew OmnThread(thisptr, "ProcThrdMonitor",
			0, true, true, __FILE__, __LINE__);
}


AosProcThrdMonitor::~AosProcThrdMonitor()
{
}


void
AosProcThrdMonitor::resetProcThrds(
		const u32 thrd_num,
		AosTransSvrThrdPtr * thrds)
{
	mThrdNum = thrd_num;
	mProcThrds = thrds;
}

bool
AosProcThrdMonitor::start()
{
	return mMonitorThrd->start();
}


bool
AosProcThrdMonitor::stop()
{
	return mMonitorThrd->stop();
}


bool
AosProcThrdMonitor::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		OmnSleep(eMonitorThrdSleep);	

		bool too_long = true;
		for(u32 i=0; i<mThrdNum; i++)
		{
			aos_assert_r(mProcThrds[i], false);
			too_long = mProcThrds[i]->isProcTooLong();
			if(!too_long)	break;
		}

		if(!too_long)	continue;
	}

	return true;
}

bool
AosProcThrdMonitor::checkThread111(OmnString &err, const int thrdLogicId)
{
	//mThrdStatus = false;
	return true;
}

bool
AosProcThrdMonitor::signal(const int threadLogicId)
{
	return true; 
}

