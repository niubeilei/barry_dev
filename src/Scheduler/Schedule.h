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
#ifndef Aos_Scheduler_Schedule_h
#define Aos_Scheduler_Schedule_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SchedulerObj.h"
#include "SEInterfaces/ScheduleObj.h"
#include "SEInterfaces/RunnableObj.h"
#include "Scheduler/Ptrs.h"
#include "Rundata/Ptrs.h"

class AosSchedule : public AosScheduleObj
{
private:
	u64						mScheduleId;

protected:
	AosSchedulerObjPtr		mScheduler;
	AosRunnableObjPtr		mRunner;
	OmnString				mVersion;
	OmnString				mJimoName;
	bool					mFinished;

public:
	AosSchedule(){};
	AosSchedule(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def);
	~AosSchedule();
	static AosSchedulePtr	createSchedule(
		const AosRundataPtr &rdata, 
		const AosSchedulerObjPtr &scheduler,
		const AosXmlTagPtr &schedule_def, 
		const AosRunnableObjPtr &runner);

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
	virtual bool scheduleFinished() = 0;
	virtual bool readyToRun(AosRundataPtr &rdata) = 0;
	virtual bool runnableStarted() = 0;
	virtual bool setRunnableFinished() = 0;

	u64 getScheduleId() const {return mScheduleId;}
	void setScheduler(const AosSchedulerObjPtr &scheduler)
	{
		mScheduler = scheduler;
	}
	void setRunner(const AosRunnableObjPtr &runner)
	{
		mRunner = runner;
	}

private:
	bool init(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def);

};
#endif

