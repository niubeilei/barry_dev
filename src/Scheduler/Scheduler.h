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
#ifndef Aos_Scheduler_Scheduler_h
#define Aos_Scheduler_Scheduler_h

#include "SEInterfaces/SchedulerObj.h"
#include "Thread/ThreadedObj.h"
#include "Scheduler/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <vector>
using namespace std;


class AosScheduler : virtual public AosSchedulerObj,
					public OmnThreadedObj
{
	OmnDefineRCObject;
protected:

	OmnMutexPtr				mLock;
	vector<AosSchedulePtr>	mSchedules;
	queue<AosSchedulePtr>	mPendingSchedules;
	AosSchedulePtr			mActiveSchedule;
	AosRundataPtr			mRundata;
	OmnThreadPtr			mThread;

public:
	AosScheduler();
	~AosScheduler();

	// Jimo Interface
	virtual bool run(const AosRundataPtr &rdata);
	virtual bool run(const AosRundataPtr &rdata, const AosXmlTagPtr &xml);
	virtual AosJimoPtr cloneJimo(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc)  const{return NULL;};
	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc){return true;}

	// Schedule Interface
	virtual u64 addSchedule(
						const AosRundataPtr &rdata, 
						const AosSchedulePtr &schedule) = 0;
	virtual bool removeSchedule(
						const AosRundataPtr &rdata, 
						const u64 schedule_id) = 0;

};
#endif

