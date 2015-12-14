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
#include "TaskScheduler/Jimos/TaskSchedulerNorm.h"

#include "TaskMgr/TaskMgr.h"
#include "TaskUtil/TaskInfo.h"
#include "TaskUtil/TaskRunnerInfo.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTaskSchedulerNorm_0(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		//AosJimoPtr jimo = OmnNew AosTaskSchedulerNorm(rdata, sdoc);
		AosJimoPtr jimo = OmnNew AosTaskSchedulerNorm();
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


AosTaskSchedulerNorm::AosTaskSchedulerNorm(const AosRundataPtr &rdata, const AosXmlTagPtr &doc)
:
AosTaskScheduler(rdata, doc)
{
}

AosTaskSchedulerNorm::AosTaskSchedulerNorm()
:
AosTaskScheduler()
{
}




AosTaskSchedulerNorm::~AosTaskSchedulerNorm()
{
}


bool
AosTaskSchedulerNorm::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	bool rslt = init(rdata);
	if (!rslt)
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
AosTaskSchedulerNorm::returnNumActive(
		const AosRundataPtr &rdata,
		const AosTaskInfoPtr &info)
{
	int physical_id = info->getTaskServerId();

	AosTaskType::E task_type = info->getTaskType();
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
	return true;
}


bool
AosTaskSchedulerNorm::nextTask(
		const AosRundataPtr &rdata, 
		const AosTaskRunnerInfoPtr &run_info)
{
	// This function is called when a machine is ready to run a task
	// or it finishes a task and is ready to run the next task. 
	//
	// PhysicalID:
	// Task Slot: 
	
	int physical_id = run_info->getTaskServerId();
	mLock->lock();
	if (physical_id < 0 || physical_id >= mNumPhysicals)
	{
		AosSetErrorU(rdata, "internal_error:") << physical_id;
		return false;
	}

	if (run_info->getTaskDocid()) 
	{
		AosTaskType::E task_type = run_info->getTaskType();
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
	}

	bool rslt = startTasksPriv(physical_id, rdata);
	mLock->unlock();
	return rslt;
}


bool
AosTaskSchedulerNorm::startTasksPriv(
		const int &physical_id,
		const AosRundataPtr &rdata)
{
	// A job has a number of tasks. There are two types of tasks:
	// (1) Map Tasks, and (2) Reduce Tasks. Each task needs zero or
	// more inputs, and generates zero or more outputs. In this
	// implementation, task outputs are always local (this is not
	// important to this scheduler). 
	//
	// 1. Tasks are managed based on their locality. Task locality
	//    is determined (mainly) by inputs. 
	//    It us
	//
	// This function starts one task on each physical machine, 
	// if possible. 'tasks_to_start' holds all the tasks being
	// scheduled by this function.
	if (mWaitTaskInfoList.empty()) return true;
	AosTaskType::E task_type;
	AosTaskInfoPtr info;
	if (mNumActiveMapTasks[physical_id] < mNumSlots)
	{
		if (mWaitTaskInfoList.empty()) return true; 
		task_type = AosTaskType::eMapTask;
		AosTaskInfoCompareFun comp(physical_id, task_type);
		mWaitTaskInfoList.sort(comp);
		print("map task", physical_id, task_type, mWaitTaskInfoList);

		while (mNumActiveMapTasks[physical_id] < mNumSlots)
		{
			if (mWaitTaskInfoList.empty()) break;
			list<AosTaskInfoPtr>::iterator itr = mWaitTaskInfoList.begin();
			info = *itr;

			if (info->getStatus() == AosTaskStatus::eFinish)
			{
				mWaitTaskInfoList.erase(itr);
				itr = mWaitTaskInfoList.begin();
				continue;
			}

			if (info->getTaskType() != task_type) break;

			mNumActiveMapTasks[physical_id]++;							

			OmnScreen << "To start task:" << physical_id << ":" 
				<< info->getTaskId() << ":" << info->getTaskDocid() << endl;

			info->setTaskServerId(physical_id);
			setTaskServerId(rdata, info->getTaskDocid(), physical_id);
			info->setStatus(AosTaskStatus::eDist);
			AosTaskMgr::getSelf()->addTask(info->getTaskDocid(), rdata);

			mWaitTaskInfoList.erase(itr);
			itr = mWaitTaskInfoList.begin();
		}
	}

	if (mNumActiveReduceTasks[physical_id] < mNumSlots)
	{
		if (mWaitTaskInfoList.empty()) return true; 
		task_type = AosTaskType::eReduceTask;
		AosTaskInfoCompareFun comp(physical_id, task_type);
		mWaitTaskInfoList.sort(comp);
		print("map task", physical_id, task_type, mWaitTaskInfoList);

		while(mNumActiveReduceTasks[physical_id] < mNumSlots)
		{
			if (mWaitTaskInfoList.empty()) break; 
			list<AosTaskInfoPtr>::iterator itr = mWaitTaskInfoList.begin();
			info = *itr;

			if (info->getStatus() == AosTaskStatus::eFinish)
			{
				mWaitTaskInfoList.erase(itr);
				itr = mWaitTaskInfoList.begin();
				continue;
			}

			if (info->getTaskType() != task_type) break;
		
			mNumActiveReduceTasks[physical_id]++;							

			OmnScreen << "To start task:" << physical_id << ":" 
				<< info->getTaskId() << ":" << info->getTaskDocid() << endl;

			info->setTaskServerId(physical_id);
			info->setStatus(AosTaskStatus::eDist);
			setTaskServerId(rdata, info->getTaskDocid(), physical_id);
			AosTaskMgr::getSelf()->addTask(info->getTaskDocid(), rdata);

			mWaitTaskInfoList.erase(itr);
			itr = mWaitTaskInfoList.begin();
		}
	}

	return true;
}


bool
AosTaskSchedulerNorm::isTaskSvrBusy(const int physical_id)
{
	if (mNumActiveMapTasks[physical_id] != 0) return false;

	if (mNumActiveReduceTasks[physical_id] != 0) return false;
	return true;
}


AosJimoPtr 
AosTaskSchedulerNorm::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosTaskSchedulerNorm(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

#endif
