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
#include "TaskScheduler/TaskSchedulerNorm.h"

#include "Rundata/Rundata.h"
#include "TaskUtil/TaskInfo.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/JobObj.h"
#include "Debug/Debug.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTaskSchedulerNorm_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosTaskSchedulerNorm(version);
		aos_assert_r(jimo, 0);
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


AosTaskSchedulerNorm::AosTaskSchedulerNorm(const u32 version)
:
AosTaskScheduler(AOSTASKSCHEDULER_NORM, version)
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
	mNumActiveTasks.clear();
	mNumActiveTasks.resize(mNumPhysicals, 0);

	mNumSlots = def->getAttrInt(AOSTAG_NUM_SLOTS, 1);
	//mNumSlots = 40; //for debugging purpose
	if (mNumSlots < 1) 
	{
		AosSetErrorU(rdata, "invalid_configuration:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (mNumSlots > eMaxSlots) mNumSlots = eMaxSlots;

	OmnTagFuncInfo << "Num of slots is: " << mNumSlots << endl;
	return true;
}


bool
AosTaskSchedulerNorm::recoverNumActive(
		const AosRundataPtr &rdata,
		const u64 &task_docid, 
		const int task_svr_id)
{
	aos_assert_r(task_svr_id >= 0 && task_svr_id < mNumPhysicals, false);
	aos_assert_r(mNumActiveTasks[task_svr_id] < mNumSlots, false);
	mNumActiveTasks[task_svr_id]++;
	return true;
}


bool
AosTaskSchedulerNorm::returnNumActive(
		const AosRundataPtr &rdata,
		const u64 &task_docid,
		const int task_svr_id)
{
	aos_assert_r(task_svr_id >= 0 && task_svr_id < mNumPhysicals, false);
	aos_assert_r(mNumActiveTasks[task_svr_id] > 0, false);
	mNumActiveTasks[task_svr_id]--;
	return true;
}


bool
AosTaskSchedulerNorm::nextTask(
		const AosRundataPtr &rdata, 
		const AosJobObjPtr &job,
		const AosTaskRunnerInfoPtr &run_info)
{
	// This function is called when a machine is ready to run a task
	// or it finishes a task and is ready to run the next task. 
	//
	// PhysicalID:
	// Task Slot: 
	
	int physical_id = run_info->getTaskRunServerId();
	mLock->lock();
	if (physical_id < 0 || physical_id >= mNumPhysicals)
	{
		AosSetErrorU(rdata, "internal_error:") << physical_id;
		return false;
	}

	if (!run_info->isIdle()) 
	{
		aos_assert_r(run_info->getTaskDocid()!= 0, false); 
		aos_assert_r(mNumActiveTasks[physical_id] > 0, false);
		mNumActiveTasks[physical_id]--;
	}

	OmnTagFuncInfo << endl; 
	bool rslt = startTasksPriv(job, physical_id, rdata);
	mLock->unlock();
	return rslt;
}

bool
AosTaskSchedulerNorm::checkRunable(const int physical_id)
{
	int num = mNumActiveTasks[physical_id];
	for (u32 i=0; i<mNumActiveTasks.size(); i++)
	{
		if (num > mNumActiveTasks[i])
		{
			return false;
		}
	}
	return true;
}


bool
AosTaskSchedulerNorm::startTasksPriv(
		const AosJobObjPtr &job,
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
	
	OmnTagFuncInfo << endl;
	sanityCheck(physical_id);
	OmnTagFuncInfo << endl;

	if (mWaitTaskInfoList.empty()) return true;
	AosTaskType::E task_type;
	AosTaskInfoPtr info;

	OmnTagFuncInfo << "active task: " << mNumActiveTasks[physical_id] 
		<< " mNumSlots: " << mNumSlots << endl;
	if (mNumActiveTasks[physical_id] < mNumSlots)
	{
		if (mWaitTaskInfoList.empty()) return true; 
		task_type = AosTaskType::eMapTask;
		AosTaskInfoCompareFun comp(physical_id, task_type);
		mWaitTaskInfoList.sort(comp);
		print("map task", physical_id, task_type, mWaitTaskInfoList);

		while (mNumActiveTasks[physical_id] < mNumSlots)
		{
			if (mWaitTaskInfoList.empty()) break;
			list<AosTaskInfoPtr>::iterator itr = mWaitTaskInfoList.begin();
			///////////////////////check runable////////////////////////////
			bool runable = false;
			while(itr != mWaitTaskInfoList.end())
			{
				info = *itr;
				u64 task_docid = info->getTaskDocid();
				aos_assert_r(task_docid != 0, false);
				AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
				aos_assert_r(task_doc, false);
				int crt_physical_id = task_doc->getAttrInt(AOSTAG_PHYSICALID, -1);
				if (crt_physical_id == -1)
				{
					runable = checkRunable(physical_id);
					break;
				}
				else if (crt_physical_id == physical_id)
				{
					runable = true;
					break;
				}
				itr++;
			}
			if (!runable)
			{
				return true;
			}
			///////////////////////check runable////////////////////////////

			if (info->getStatus() == AosTaskStatus::eFinish)
			{
				mWaitTaskInfoList.erase(itr);
				itr = mWaitTaskInfoList.begin();
				continue;
			}

			mNumActiveTasks[physical_id]++;							

			OmnScreen << "To start task:" << physical_id << ":" 
				<< info->getTaskId() << ":" << info->getTaskDocid() << endl;

			OmnTagFuncInfo << "To start task:" << physical_id << ":" 
				<< info->getTaskId() << ":" << info->getTaskDocid() << endl;
			startTask(job, info, physical_id, rdata);
			mWaitTaskInfoList.erase(itr);
			itr = mWaitTaskInfoList.begin();
		}
	}

	OmnTagFuncInfo << endl;

	return true;
}


AosJimoPtr 
AosTaskSchedulerNorm::cloneJimo()  const
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


bool
AosTaskSchedulerNorm::sanityCheck(const int physicalid)
{
	AosJobMgrObj::ProcStatus sts = AosJobMgrObj::getJobMgr()->getProcStatus(physicalid);	
	aos_assert_r(sts != AosJobMgrObj::eUnusable, false);
	return true;
}

u32 
AosTaskSchedulerNorm::getNumSlots()
{
	return mNumSlots;
}
