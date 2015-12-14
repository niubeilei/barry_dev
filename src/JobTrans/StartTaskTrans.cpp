////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/StartTaskTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "Thread/ThreadPool.h"
#include "TaskMgr/Task.h"
#include "TaskMgr/TaskThrd.h"
#include "Debug/Debug.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("StartTaskTrans", __FILE__, __LINE__);
static OmnThreadPoolPtr sgRunTaskThreadPool = OmnNew OmnThreadPool("StartTaskTrans", __FILE__, __LINE__);

AosStartTaskTrans::AosStartTaskTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eStartTask, regflag)
{
}


AosStartTaskTrans::AosStartTaskTrans(
		const u64 &task_docid,
		const int svr_id,
		const u32 to_proc_id)
:
AosTaskTrans(AosTransType::eStartTask, svr_id, to_proc_id, false, false),
mTaskDocid(task_docid)
{
}

AosStartTaskTrans::~AosStartTaskTrans()
{
}

bool
AosStartTaskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid, false);
	return true;
}


bool
AosStartTaskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosStartTaskTrans::clone()
{
	return OmnNew AosStartTaskTrans(false);
}


bool
AosStartTaskTrans::proc()
{
OmnScreen << "===========================start task trans: "
	      << mTaskDocid
		  << " : logic pid: " << getToProcId()
		  << " : process id: " << getpid()
		  << endl;
OmnTagFuncInfo << "===========================start task trans: "
	      << mTaskDocid
		  << " : logic pid: " << getToProcId()
		  << endl;
	aos_assert_r(mTaskDocid, false);
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, mRdata);
	aos_assert_r(task, false);
	mRdata->setOk();
	task->setRundata(mRdata);
	task->initOutPut(mRdata);
	OmnTagFuncInfo << "proc a task trans.taskdocid is: " << mTaskDocid << endl;
	return startTask(mRdata);
}


OmnString
AosStartTaskTrans::toString()
{
	OmnString str = "Start Task By Docid:";
	str << mTaskDocid;
	return str;
}

bool
AosStartTaskTrans::startTask(
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(!(task->isFinished()), false);
	aos_assert_r(task, false);
	aos_assert_r(rdata, false);
	try
	{
		//if (task->getVersion() == 1 && !task->isService())
		//{
		if (task->getVersion() == 1)
		{
			OmnTagFuncInfo << "trigger sgThreadPool->proc for: " 
				<< task->getTaskDocid() << endl;
			sgThreadPool->proc(OmnNew StartTaskThrd(mTaskDocid, rdata));
		}
		else
		{
			OmnTagFuncInfo << "trigger AosTaskThrd for:" 
				<< task->getTaskDocid() << endl;
			AosTaskThrdPtr task_thread = OmnNew AosTaskThrd(mTaskDocid, rdata);
		}
	}
	catch (...)
	{
		AosSetErrorU(rdata, "failed_create_taskthrd");
		task->taskFailed(rdata);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

bool
AosStartTaskTrans::FinishTaskThrd::run()
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, mRundata);
	aos_assert_r(task, false);
	bool timeout;
	//if (mTask->allActionIsFinished(mRundata))
	//{
		for(u32 i=0; i<mThrdNum; i++)
		{
			do{
				timeout = false;
				mSem->timedWait(eReportWaitTime, timeout);
				if (timeout)
					task->report();
			}while(timeout);
		}

		task->finishedAction(mRundata);
		task->taskFinished(mRundata);
		task->report();
	//}
		AosTask::resetTask();
	return true;
}

bool
AosStartTaskTrans::FinishTaskThrd::procFinished()
{
	return true;
}

bool
AosStartTaskTrans::StartTaskThrd::run()
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, mRundata);
	aos_assert_r(task, false);
	OmnTagFuncInfo << "start trans" << endl;
	task->taskStarted(mRundata);
	u32 t1 = OmnGetSecond();
	bool rslt = task->initMemberAction(mRundata);
	aos_assert_r(rslt, false);
	rslt = task->createSnapShot(mRundata);
	aos_assert_r(rslt, false);
	u32 t2 = OmnGetSecond();
	if (t2-t1 > 5)
	{
		OmnAlarm << "Too long time to init task!" << enderr;
	}

	int n = task->getResource(AOSTAG_MAXNUMTHREAD, mRundata);
	OmnSemPtr sem = OmnNew OmnSem(0);
	
	OmnTagFuncInfo << "run finish trans" << endl;
	sgThreadPool->proc(OmnNew FinishTaskThrd(mTaskDocid, sem, n, mRundata));

	for (int i=0; i<n; i++)
	{
		OmnTagFuncInfo << "proc run task trans" << endl;
		sgRunTaskThreadPool->proc(OmnNew RunTaskThrd(mTaskDocid, sem, mRundata));
	}
	return true;
}


bool
AosStartTaskTrans::StartTaskThrd::procFinished()
{

	return true;
}


bool
AosStartTaskTrans::RunTaskThrd::run()
{
	void *data = 0;;
	void *datacheck = 0;
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, mRundata);
	aos_assert_r(task, false);
	task->onThreadInit(mRundata, &data);
	if (data) datacheck = data;
	while(!task->isTaskRunFinished())
	{
		u32 t1 = OmnGetSecond();
		bool rslt = task->runAction(mRundata, data);
		aos_assert_r(rslt, false);
		u32 t2 = OmnGetSecond();
		if (t2-t1 > 10)
		{
			//OmnAlarm << "Too long time to run a time slice task!" << enderr;
		}
	}
	aos_assert_r(data == datacheck, false);
	task->onThreadExit(mRundata, data);
	//aos_assert_r(data==0, false);
	mSem->post();
	return true;
}


bool
AosStartTaskTrans::RunTaskThrd::procFinished()
{
	return true;
}
