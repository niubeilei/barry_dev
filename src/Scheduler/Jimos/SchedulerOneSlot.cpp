////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Scheduler/Jimos/SchedulerOneSlot.h"

#include "Scheduler/Schedule.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "SEInterfaces/Ptrs.h"
#include "Porting/Sleep.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchedulerOneSlot_0(
 		const AosRundataPtr &rdata, 
 		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchedulerOneSlot(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


// Chen Ding, 2013/01/12
AosSchedulerOneSlot::AosSchedulerOneSlot(const u32 version)
{
}


AosSchedulerOneSlot::~AosSchedulerOneSlot()
{
}

u64
AosSchedulerOneSlot::addSchedule(
		const AosRundataPtr &rdata, 
		const AosSchedulePtr &schedule)
{
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mLock->lock();
	mSchedules.push_back(schedule);
	mLock->unlock();
	return schedule->getScheduleId();
}


bool	
AosSchedulerOneSlot::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1);
		mThreadStatus = true;
		checkSchedules();
		mThreadStatus = true;
	}

	return true;
}


bool	
AosSchedulerOneSlot::signal(const int threadLogicId)
{
	mThreadStatus = false;
	return true;
}


bool
AosSchedulerOneSlot::checkSchedules()
{
	mLock->lock();
	u32 i=0;
	while (i < mSchedules.size())
	{
		if (mSchedules[i]->readyToRun(mRundata))
		{
			AosSchedulePtr schedule = mSchedules[i];
			mPendingSchedules.push(schedule);
			if (i < mSchedules.size()-1)
			{
				mSchedules[i] = mSchedules[mSchedules.size()-1];
			}
			mSchedules.pop_back();
		}
		else
		{
			i++;
		}
	}

	while (mPendingSchedules.size() > 0 && !mActiveSchedule)
	{
		mActiveSchedule = mPendingSchedules.front();
		mPendingSchedules.pop();
		aos_assert_rl(mActiveSchedule, mLock, false);
		mActiveSchedule->runnableStarted();
		mActiveSchedule->run(mRundata);
		mActiveSchedule->setRunnableFinished();
		if (!mActiveSchedule->scheduleFinished())
		{
			mSchedules.push_back(mActiveSchedule);
		}
		mActiveSchedule = 0;
	}
	mLock->unlock();
	return true;
}

bool 
AosSchedulerOneSlot::run(const AosRundataPtr &rdata) 
{
    OmnThreadedObjPtr thisPtr(this, false); 
	mThread = OmnNew OmnThread(thisPtr, "ScheduleThreadObj", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}

bool 
AosSchedulerOneSlot::run(const AosRundataPtr &rdata, const AosXmlTagPtr &xml) 
{
	return true;
}

bool 
AosSchedulerOneSlot::removeSchedule(
		const AosRundataPtr &rdata, 
		const u64 schedule_id)
{
	OmnNotImplementedYet;
	return false;
}

AosJimoPtr 
AosSchedulerOneSlot::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosSchedulerOneSlot(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

