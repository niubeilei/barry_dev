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
#include "TaskScheduler/TaskScheduler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Buff.h"
#include "Job/Job.h"
#include "TaskMgr/TaskMgr.h"
#include "TaskScheduler/TaskSchedulerNorm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"


AosTaskScheduler::AosTaskScheduler(
		const OmnString &type,
		const u32 version)
:
AosTaskSchedulerObj(version),
mType(type)
{
}


AosTaskScheduler::AosTaskScheduler()
:
mLock(OmnNew OmnMutex())
{
}


AosTaskScheduler::~AosTaskScheduler()
{
}


AosTaskSchedulerPtr
AosTaskScheduler::createTaskScheduler(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def)
{
	/*
	OmnString jimo_scheduler_config = "<jimo zky_objid=\"jimo_123\" ";
	jimo_scheduler_config << "zky_classname=\"AosTaskSchedulerNorm\" "
		<< "zky_otype=\"zkyotp_jimo\" "
		<< "current_version=\"1.0\">"
		<< "<versions>"
		<< "<ver_0>libTaskSchedulerJimos.so</ver_0>"
		<< "</versions>"
		<< "</jimo>";
	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata, jimo_scheduler_config AosMemoryCheckerArgs);
	aos_assert_r(jimo_doc, 0);

	AosJimoPtr jimo = AosCreateJimo(rdata, schedule_def, jimo_doc);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_creating_scheduler") 
			<< schedule_def->toString() << enderr;
		return 0;
	}

	AosTaskSchedulerPtr scheduler = dynamic_cast<AosTaskScheduler*>(jimo.getPtr());
	*/
	OmnTagFuncInfo << endl;
	AosTaskSchedulerPtr scheduler = OmnNew AosTaskSchedulerNorm();
	if (!scheduler)
	{
		AosSetErrorUser(rdata, "invalid_schedule") 
			<< schedule_def->toString() << enderr;
		return 0;
	}

	scheduler->config(rdata, schedule_def);
	return scheduler;
}


bool
AosTaskScheduler::addTasks(
		const AosRundataPtr &rdata, 
		const u64 &job_docid,
		const vector<AosTaskInfoPtr> &tasks)
{
	// This function is called when the task creator created some
	// tasks. It adds the tasks to this scheduler.
	//aos_assert_r(!tasks.empty(), false);
	if (tasks.empty())
	{
		OmnScreen << "Has no Task to add, job_docid: " << job_docid << endl;
		return true;
	}
	mLock->lock();
	for (u32 i = 0; i < tasks.size(); i++)
	{
		OmnScreen << "Wait Wait Wait task_docid: " << tasks[i]->getTaskDocid()<< endl;
		aos_assert_r(tasks[i]->getStatus() == AosTaskStatus::eWait, false);
		mWaitTaskInfoList.push_back(tasks[i]);
	}
	mLock->unlock();
	OmnTagFuncInfo << "totally tasks: " << tasks.size() << endl;
	return true;
}


bool
AosTaskScheduler::removeWaitListByTaskId(const u64 &task_docid)
{
	list<AosTaskInfoPtr>::iterator itr = mWaitTaskInfoList.begin();
	for (; itr != mWaitTaskInfoList.end(); ++itr)
	{
		if ((*itr)->getTaskDocid() == task_docid) 
		{
			mWaitTaskInfoList.erase(itr);
			break;
		}
	}
	return true;
}


bool
AosTaskScheduler::startTask(
		const AosJobObjPtr &job, 
		const AosTaskInfoPtr &info,
		const int physical_id,
		const AosRundataPtr &rdata)
{
	u64 task_docid = info->getTaskDocid();
	u64 timestamp = OmnGetTimestamp();
	OmnString logic_id = info->getLogicId();
	AosLogicTaskObjPtr logic_task = job->getLogicTask(logic_id, rdata);
	aos_assert_r(logic_task, false);

	logic_task->taskStarted(task_docid, timestamp, physical_id, rdata);
	aos_assert_r(AosGetSelfServerId() == job->getJobPhysicalId(), false);

	OmnTagFuncInfo << endl;

	//Linda, 2014/05/08
	AosXmlTagPtr snapshots_tag = 0;
	if (info->needSnapShot())
	{
		snapshots_tag = job->getSnapShots(rdata);

		if (snapshots_tag)
		{
			AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
			task_doc->addNode(snapshots_tag);

			bool rslt = AosModifyDoc(task_doc, rdata);
			aos_assert_r(rslt, false);
		}

	}
	AosTaskMgrObj::addTaskStatic(job->getJobPhysicalId(), task_docid, rdata);
	return true;
}


bool
AosTaskScheduler::init(const AosRundataPtr &rdata)
{
	mNumPhysicals = AosGetNumPhysicals();
	if (mNumPhysicals <= 0)
	{
		return false;
	}
	return true;
}


void
AosTaskScheduler::print(
		const OmnString &name,
		const int phy, 
		const AosTaskType::E task_type,
		list<AosTaskInfoPtr> &vv)
{
	int i = 0;
	if (!i) return;
	list<AosTaskInfoPtr> ::iterator itr;
	for (itr = vv.begin(); itr != vv.end(); ++itr)
	{
		AosTaskInfoPtr task = *itr;
		OmnString docstr;
		docstr  << name  
			<< " phy :" << phy
			<< "; task_type: " << task_type
			<< "; task_docid:" << task->getTaskDocid()
			<< "; optimalphy: " << task->getOptimalPhyId()
			<< "; type: " << task->getTaskType() 
			<< "; id:" << task->getTaskId();
		OmnScreen << docstr << endl;
	}
}


u32 
AosTaskScheduler::getNumSlots()
{
	OmnNotImplementedYet;
	return 0;
}
