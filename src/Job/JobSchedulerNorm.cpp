////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/15/2012 Created by Ketty 
// 06/02/2012 Added 'mNumSlots', tag: Change06022012_1
//   mNumSlots means how many tasks a job can run concurrently on one 
//   machine. If 'mNumSlots' is 1, it means only one task can run on
//   one machine. If there are multiple tasks for that machine, when 
//   one task finishes, it can kick off the next one.
// 06/02/2012 Added 'mStartType', tag: Change06022012_2
//   When starting tasks, it should check whether a task can be started
//   when its job is started. There are three values for mStartType:
//   	eAuto: 		it can be started whenever its job is started
//   	eToken:		it will be started by a token
//   	eChain:		it will be started by its previous task
//
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSchedulerNorm.h"

#include "API/AosApi.h"
#include "TaskMgr/TaskMgr.h"
#include "TaskUtil/TaskInfo.h"
#include "Job/JobMgr.h"


AosJobSchedulerNorm::AosJobSchedulerNorm(const bool flag)
:
AosJobScheduler(AosJobSchedulerId::eNorm, AOSJOBSCHEDULER_NORM, flag),
mLock(OmnNew OmnMutex())
{
}


AosJobSchedulerNorm::~AosJobSchedulerNorm()
{
}


bool
AosJobSchedulerNorm::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	mNumPhysicals = AosGetNumPhysicals();
	if (mNumPhysicals <= 0)
	{
		AosSetErrorU(rdata, "num of physicals is error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mNumActiveMapTasks.clear();
	mNumActiveMapTasks.resize(mNumPhysicals, 0);

	mNumActiveReduceTasks.clear();
	mNumActiveReduceTasks.resize(mNumPhysicals, 0);

	mNumSlots = def->getAttrInt(AOSTAG_NUM_SLOTS, 1);
	if (mNumSlots < 1) 
	{
		AosSetErrorU(rdata, "invalid_configuration:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (mNumSlots > eMaxSlots) mNumSlots = eMaxSlots;
	return true;
}


bool
AosJobSchedulerNorm::startTasks(
		list<AosTaskInfoPtr> &task_list,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	bool rslt = startTasksPriv(task_list, rdata);
	mLock->unlock();
	return rslt;
}

	
bool
AosJobSchedulerNorm::nextTask(
		list<AosTaskInfoPtr> &task_list,
		const AosTaskInfoPtr &finished_task,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	bool rslt = nextTaskPriv(task_list, finished_task, rdata);
	mLock->unlock();
	return rslt;
}


bool
AosJobSchedulerNorm::startTasksPriv(
		list<AosTaskInfoPtr> &task_list,
		const AosRundataPtr &rdata)
{
	// This function starts one task on each physical machine, 
	// if possible. 'tasks_to_start' holds all the tasks being
	// scheduled by this function.

	int physical_id = 0;
	AosTaskType::E task_type;
	AosTaskInfoPtr info;
	list<AosTaskInfoPtr>::iterator itr = task_list.begin();
	while(itr != task_list.end())
	{
		info = *itr;

		if (info->getStatus() == AosTaskStatus::eFinish)
		{
			task_list.erase(itr);
			itr = task_list.begin();
			continue;
		}

		physical_id = info->getPhysicalId();
		if (physical_id < 0 || physical_id >= mNumPhysicals)
		{
			task_list.erase(itr);
			itr = task_list.begin();
			AosSetErrorU(rdata, "internal_error:") << physical_id << enderr;
			continue;
		}

		task_type = info->getTaskType();
		if (task_type == AosTaskType::eMapTask && mNumActiveMapTasks[physical_id] < mNumSlots)
		{
			mNumActiveMapTasks[physical_id]++;							

			OmnScreen << "To start task:" << physical_id << ":" 
				<< info->getTaskId() << ":" << info->getTaskDocid() << endl;
			AosTaskMgr::getSelf()->addTask(info->getTaskDocid(), rdata);

			task_list.erase(itr);
			itr = task_list.begin();
			continue;
		}

		if (task_type == AosTaskType::eReduceTask && 
			mNumActiveReduceTasks[physical_id] < mNumSlots)
		{
			mNumActiveReduceTasks[physical_id]++;							

			OmnScreen << "To start task:" << physical_id << ":" 
				<< info->getTaskId() << ":" << info->getTaskDocid() << endl;
			AosTaskMgr::getSelf()->addTask(info->getTaskDocid(), rdata);

			task_list.erase(itr);
			itr = task_list.begin();
			continue;
		}

		itr++;
	}

	return true;
}


bool
AosJobSchedulerNorm::nextTaskPriv(
		list<AosTaskInfoPtr> &task_list,
		const AosTaskInfoPtr &finished_task,
		const AosRundataPtr &rdata)
{
	// 'finished_task' finished its execution. It checks whether the 
	// task has a synchronize token. If no, it can pick the next task
	// to run. Otherwise, it decrements the token. If the token becomes
	// zero, it can kick off the next task (determined by the token)
	// to run.
	
	int physical_id = finished_task->getPhysicalId();
	if (physical_id < 0 || physical_id >= mNumPhysicals)
	{
		AosSetErrorU(rdata, "internal_error:") << physical_id;
		return false;
	}

	AosTaskType::E task_type = finished_task->getTaskType();
	if (task_type == AosTaskType::eMapTask)
	{
		aos_assert_r(mNumActiveMapTasks[physical_id] > 0, false);
		mNumActiveMapTasks[physical_id]--;
	}
	else if (task_type == AosTaskType::eReduceTask)
	{
		aos_assert_r(mNumActiveReduceTasks[physical_id] > 0, false);
		mNumActiveReduceTasks[physical_id]--;
	}

	return startTasksPriv(task_list, rdata);
}


bool 
AosJobSchedulerNorm::rerunTask(
		const AosTaskObjPtr &task, 
		const AosRundataPtr &rdata)
{
	// The task 'task' failed running on its 'home' physical machine. 
	// This function determines an alternative physical machine to 
	// re-run the task.
	//
	// In the current implementations, simply re-run it on the next
	// physical machine.
	OmnNotImplementedYet;
	return false;
}


AosJobSchedulerPtr 
AosJobSchedulerNorm::create(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata) const
{
	AosJobSchedulerNorm * sch = OmnNew AosJobSchedulerNorm(false);
	bool rslt = sch->config(sdoc, rdata);
	if (!rslt) return 0;
	return sch;
}

#endif 
