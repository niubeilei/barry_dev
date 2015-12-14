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
// 2013/10/31 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TaskScheduler_h
#define Aos_SEInterfaces_TaskScheduler_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskUtil/Ptrs.h"

#define AOSTASKSCHEDULER_DUMMY					"dummy"
#define AOSTASKSCHEDULER_NORM					"norm"

class AosTaskSchedulerObj : public AosJimo
{
public:
	AosTaskSchedulerObj();
	AosTaskSchedulerObj(const u32 version);
	~AosTaskSchedulerObj();

	virtual bool config(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &def) = 0;

	virtual bool addTasks(
				const AosRundataPtr &rdata, 
				const u64 &task_docid,
				const vector<AosTaskInfoPtr> &tasks) = 0;

	virtual bool nextTask(
				const AosRundataPtr &rdata,
				const AosJobObjPtr &job,
				const AosTaskRunnerInfoPtr &info) = 0;

	virtual bool returnNumActive(
				const AosRundataPtr &rdata,
				const u64 &task_docid,
				const int task_svr_id) = 0;

	virtual bool recoverNumActive(
				const AosRundataPtr &rdata,
				const u64 &task_docid,
				const int task_svr_id) = 0;

	virtual bool removeWaitListByTaskId(const u64 &task_docid) = 0;
};

#endif

