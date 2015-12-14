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
#ifndef Aos_TaskScheduler_TaskSchedulerDummy_h
#define Aos_TaskScheduler_TaskSchedulerDummy_h

#include "TaskScheduler/TaskScheduler.h"

class AosTaskSchedulerDummy : public AosTaskScheduler 
{
	OmnDefineRCObject;
public:
	AosTaskSchedulerDummy();
	AosTaskSchedulerDummy(const u32 version);
	~AosTaskSchedulerDummy();

	// TaskScheduler Interface
	virtual bool nextTask(
				const AosRundataPtr &rdata,
				const AosJobObjPtr &job,
				const AosTaskRunnerInfoPtr &run_info);

	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &def);

	virtual bool returnNumActive(
				const AosRundataPtr &rdata,
				const int task_svr_id);

	virtual bool recoverNumActive(
				const AosRundataPtr &rdata,
				const int task_svr_id);

};
#endif

