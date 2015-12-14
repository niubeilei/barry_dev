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
#ifndef Aos_Scheduler_SchedulerOneSlot_h
#define Aos_Scheduler_SchedulerOneSlot_h

#include "Scheduler/Scheduler.h"
#include "Thread/ThreadedObj.h"
#include "Scheduler/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <vector>
using namespace std;


class AosSchedulerOneSlot : virtual public  AosScheduler
{
	OmnDefineRCObject;
private:

	bool	checkSchedules();

public:
	AosSchedulerOneSlot();
	~AosSchedulerOneSlot();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

	// Jimo Interface
	virtual bool run(const AosRundataPtr &rdata);
	virtual bool run(const AosRundataPtr &rdata, const AosXmlTagPtr &xml);
	virtual AosJimoPtr cloneJimo(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc)  const;
	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc){return true;}


	virtual u64 addSchedule(
						const AosRundataPtr &rdata, 
						const AosSchedulePtr &schedule);
	virtual bool removeSchedule(
						const AosRundataPtr &rdata, 
						const u64 schedule_id);

};
#endif

