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
#ifndef Aos_TaskScheduler_TaskScheduler_h
#define Aos_TaskScheduler_TaskScheduler_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "SEInterfaces/TaskSchedulerObj.h"
#include "TaskUtil/TaskInfo.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "TaskScheduler/Ptrs.h"
#include "Job/Job.h"
#include <vector>
#include <list>
#include <map>
using namespace std;

class AosTaskScheduler : public AosTaskSchedulerObj 
{
protected:
	OmnMutexPtr				mLock;
	int						mNumPhysicals;
	list<AosTaskInfoPtr>    mWaitTaskInfoList;
	OmnString				mType;
	AosJobObjPtr			mJob;

public:
	AosTaskScheduler();
	AosTaskScheduler(const OmnString &type, const u32 version); 
	~AosTaskScheduler();

	virtual AosJimoPtr cloneJimo()  const{OmnNotImplementedYet; return NULL;};

	// TaskScheduler Interface
	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &def) = 0;

	void setJob(AosJobObjPtr job) { mJob = job; }

	bool addTasks(
				const AosRundataPtr &rdata, 
				const u64 &job_docid,
				const vector<AosTaskInfoPtr> &tasks);

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

	virtual u32 getNumSlots();

	bool removeWaitListByTaskId(const u64 &task_docid);

	static AosTaskSchedulerPtr createTaskScheduler(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &def);

protected:
	void 	print(
				const OmnString &name,
				const int phy, 
				const AosTaskType::E task_type,
				list<AosTaskInfoPtr> &vv);

	bool	init(const AosRundataPtr &rdata);

	bool	startTask(
				const AosJobObjPtr &job, 
				const AosTaskInfoPtr &info,
				const int physical_id,
				const AosRundataPtr &rdata);

};
#endif

