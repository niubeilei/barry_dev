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
#include "Scheduler/Schedule.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"

#include "SEInterfaces/Ptrs.h"
#include "API/AosApiC.h"

AosSchedule::AosSchedule(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def)
:
AosScheduleObj(rdata, schedule_def),
mScheduleId(OmnGetTimestamp()),
mFinished(false)
{
	bool rslt = init(rdata, schedule_def);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}

AosSchedule::~AosSchedule()
{
}


bool
AosSchedule::init(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def)
{
	return true;
}


AosSchedulePtr
AosSchedule::createSchedule(
		const AosRundataPtr &rdata, 
		const AosSchedulerObjPtr &scheduler,
		const AosXmlTagPtr &schedule_def, 
		const AosRunnableObjPtr &runner)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, schedule_def);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_creating_scheduler") 
			<< schedule_def->toString() << enderr;
		return 0;
	}

	AosSchedulePtr schedule = dynamic_cast<AosSchedule*>(jimo.getPtr());
	if (!schedule)
	{
		AosSetErrorUser(rdata, "invalid_schedule") 
			<< schedule_def->toString() << enderr;
		return 0;
	}

	schedule->setScheduler(scheduler);
	schedule->setRunner(runner);
	return schedule;
}

bool 
AosSchedule::run(const AosRundataPtr &rdata) 
{
	aos_assert_rr(mRunner, rdata, false);
	mRunner->run(rdata);
	return true;
}

bool 
AosSchedule::run(const AosRundataPtr &rdata, const AosXmlTagPtr &xml) 
{
	aos_assert_rr(mRunner, rdata, false);
	mRunner->run(rdata);
	return true;
}


