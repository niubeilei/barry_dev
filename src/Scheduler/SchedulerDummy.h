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
// 2013/10/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Scheduler_SchedulerDummy_h
#define Aos_Scheduler_SchedulerDummy_h

#include "Scheduler/Scheduler.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <vector>
using namespace std;


class AosSchedulerDummy : virtual public AosScheduler
{
	OmnDefineRCObject;

public:
	AosSchedulerDummy();
	~AosSchedulerDummy();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);


	// Schedule Interface
	virtual u64 addSchedule(
						const AosRundataPtr &rdata, 
						const AosSchedulePtr &schedule);
	virtual bool removeSchedule(
						const AosRundataPtr &rdata, 
						const u64 schedule_id);

    virtual bool      	start();
};
#endif

