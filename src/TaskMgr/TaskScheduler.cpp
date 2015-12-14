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
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskMgr/TaskScheduler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "SEInterfaces/SmartDocObj.h"
#include "TaskMgr/MapTask.h"
#include "TaskMgr/TaskThrd.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"



AosTaskScheduler::AosTaskScheduler()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosTaskScheduler::~AosTaskScheduler()
{
}


bool
AosTaskScheduler::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "task_scheduler", 0, true, true, __FILE__, __LINE__);
    mThread->start();
	return true;
}


bool
AosTaskScheduler::stop()
{
    return true;
}


bool
AosTaskScheduler::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosTaskScheduler::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mPendingTasks.size() <= 0)
		{
			// queryDb();
			if (mPendingTasks.size() <= 0)
			{
				mCondVar->wait(mLock);
				mLock->unlock();
				continue;
			}
		}

		scheduleTasks();
		mLock->unlock();
	}
	return true;
}


bool
AosTaskScheduler::addRequest(
		const AosXmlTagPtr &request, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskScheduler::scheduleTasks()
{
	OmnNotImplementedYet;
	return false;
}
#endif
