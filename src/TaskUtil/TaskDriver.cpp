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
// 2013/06/18	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "TaskUtil/TaskDriver.h"

#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Job/Job.h"
#include "Job/JobMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "TaskUtil/TaskUtil.h"
#include "JobTrans/GetTaskRunnerInfoTrans.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Debug/Debug.h"

OmnSingletonImpl(AosTaskDriverSingleton,
                 AosTaskDriver,
                 AosTaskDriverSelf,
                "AosTaskDriver");

AosTaskDriver::AosTaskDriver()
:
mHbTimerSec(eDefaultHbTimerSec),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mThread(0),
mHbCount(0)
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "taskdriver", 0, false, true, __FILE__, __LINE__);
	mThread->start();
}


AosTaskDriver::~AosTaskDriver()
{
}


bool
AosTaskDriver::start()
{
	return true;
}


bool
AosTaskDriver::config(const AosXmlTagPtr &conf)
{
	AosXmlTagPtr def = conf->getFirstChild("taskdriver");
	mHbTimerSec = def->getAttrInt("hb_timesec", eDefaultHbTimerSec);

	//change to use 5 second for fast task scheduling
	//mHbTimerSec = 5;

	OmnTagFuncInfo << "heartbeat timer: " << eDefaultHbTimerSec << endl;
	return true;
}


bool
AosTaskDriver::stop()
{
	return true;
}

bool
AosTaskDriver::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	int t1 = 0;
	int hb_timersec = mHbTimerSec;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		EventTrigger event;
		AosTaskDriverRequestPtr req;

		int timecost = OmnGetSecond() - t1;
OmnScreen << "999999999999999999999999999 timecost: " << timecost << " , " << hb_timersec << endl;
		if (timecost >= hb_timersec)
		{
			event = eSignalHeartbeat;
			mLock->unlock();
		}
		else
		{
			int tt = hb_timersec - timecost;
			if (mQueue.empty())
			{
				bool timeout = false;
				mCondVar->timedWait(mLock, timeout, tt, 0);
				if (timeout && tt == hb_timersec)
				{
					mHbCount++;
					if (mHbCount % 5 == 0) hb_timersec += 30;
				}
				mLock->unlock();
				continue;
			}

			if (mHbCount != 0)
			{
				mHbCount = 0;
				hb_timersec = mHbTimerSec;
			}
			
			req = mQueue.front();
			mQueue.pop(); 
			event = req->mEvent;
			mLock->unlock();
		}
		switch(event)
		{
		case eUpdateTaskStatus:
			 updateTaskStatus(req);
			 break;

		case eUpdateTaskProgress:
			 updateTaskProgress(req);
			 break;

		case eScheduleNextTask:
			 OmnTagFuncInfo << endl;
			 scheduleNextTask(req); 
			 break;
		
		case eNotifyJobTaskStop:
			 getTaskRunnerInfoBySvrId(req);
			 break;

		case eSvrDeath:
			 AosJobMgrObj::getJobMgr()->svrDeath(req->mServerId);
			 break;

		case eSignalHeartbeat:
			 AosJobMgrObj::getJobMgr()->checkStartJobs();
			 OmnTagFuncInfo << endl;
			 AosJobMgrObj::getJobMgr()->sendHeartbeat();
			 t1 = OmnGetSecond();
			 break;

		case eStopJob:
			 stopJobPriv(req);
			 break;

		default:
			 OmnAlarm << "AAAAAAAAAAAAA" << enderr;
			 break;
		}
	}
	return true;
}


bool
AosTaskDriver::signal(const int threadLogicId)
{
	return true;
}


bool
AosTaskDriver::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosTaskDriver::addEvent(const AosTaskDriverRequestPtr &req)
{
	mLock->lock();
	mQueue.push(req);	
	mCondVar->signal();
	mLock->unlock();
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosTaskDriver::updateTaskStatus(const AosTaskDriverRequestPtr &req)
{
	AosTaskRunnerInfoPtr runner_info = req->mRunnerinfo;
	AosJobObjPtr job = getStartJob(runner_info, req->mRdata);
	if (!job) return true;

	AosRundataPtr rdata = job->getRundata();
	AosLogicTaskObjPtr logic_task = job->getLogicTask(runner_info->getLogicId(), rdata);
	aos_assert_r(logic_task, false);

	logic_task->updateTaskStatus(job, runner_info, rdata);
	return true;
}


bool
AosTaskDriver::updateTaskProgress(const AosTaskDriverRequestPtr &req)
{
	AosTaskRunnerInfoPtr runner_info = req->mRunnerinfo;
	AosJobObjPtr job = getStartJob(runner_info, req->mRdata);
	if (!job) return true;

	AosRundataPtr rdata = job->getRundata();
	AosLogicTaskObjPtr logic_task = job->getLogicTask(runner_info->getLogicId(), rdata);
	aos_assert_r(logic_task, false);

	logic_task->updateTaskProgress(job, runner_info->getTaskDocid(), runner_info->getProgress(), rdata);
	return true;
}


bool
AosTaskDriver::scheduleNextTask(const AosTaskDriverRequestPtr &req)
{
	OmnTagFuncInfo << endl;
	AosTaskRunnerInfoPtr runner_info = req->mRunnerinfo;
	return AosJobMgrObj::getJobMgr()->scheduleNextTask(runner_info); 
}


bool
AosTaskDriver::getTaskRunnerInfoBySvrId(const AosTaskDriverRequestPtr &req)
{
	return AosJobMgrObj::getJobMgr()->getTaskRunnerInfoBySvrId(req->mRdata, req->mServerId);
}


bool
AosTaskDriver::stopJob(const u64 &job_docid, const AosRundataPtr &rdata)
{
	AosTaskDriverRequestPtr req = OmnNew AosTaskDriverRequest(
			AosTaskDriver::eStopJob, job_docid, rdata);
	return AosTaskDriver::getSelf()->addEvent(req);
}


bool
AosTaskDriver::stopJobPriv(const AosTaskDriverRequestPtr &req)
{
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(req->mJobDocid, req->mRdata);
	aos_assert_r(job, false);
	AosRundataPtr rdata = job->getRundata();
	return job->stopJob(rdata);
}


bool
AosTaskDriver::scheduleTasks()
{
	OmnTagFuncInfo << endl;
	AosTaskDriverRequestPtr req = OmnNew AosTaskDriverRequest(
			AosTaskDriver::eSignalHeartbeat);
	return AosTaskDriver::getSelf()->addEvent(req);
}


bool
AosTaskDriver::addSvrDeathEvent(const int svr_id)
{
	AosTaskDriverRequestPtr req = OmnNew AosTaskDriverRequest(
			AosTaskDriver::eSvrDeath, svr_id);
	return AosTaskDriver::getSelf()->addEvent(req);
}


AosJobObjPtr
AosTaskDriver::getStartJob(
		const AosTaskRunnerInfoPtr &runner_info,
		const AosRundataPtr &rdata)
{
	u64 job_docid = runner_info->getJobDocid();
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(job_docid, rdata);
	if (!job)
	{
		OmnScreen << "Job not in Running!!! job_docid: " << job_docid << " , task_docid: " << runner_info->getTaskDocid()<< endl;
		//AosTaskUtil::stopTaskProcess(runner_info->getTaskServerId(), runner_info->getTaskDocid(), rdata);
		return 0;
	}
	if (job->isStoped()||job->isFailed())
	{
		OmnScreen << "Job stop, failed!!! job_docid: " << job_docid << " , task_docid: " << runner_info->getTaskDocid()<< endl;
		AosTaskUtil::stopTaskProcess(runner_info->getTaskRunServerId(), runner_info->getTaskDocid(), rdata);
		return 0;
	}

	return job;
}

