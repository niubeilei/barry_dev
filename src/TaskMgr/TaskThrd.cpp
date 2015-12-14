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
// 04/29/2012: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/TaskThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "TaskMgr/MapTask.h"
#include "TaskMgr/TaskLogKeys.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "Debug/Debug.h"


static u64 sgTotalTasksProcessed = 0;
static OmnMutex sgTotalLock;

static map<u64, AosTaskThrd*> sgTaskThrdMap;

AosTaskThrd::AosTaskThrd(
		const u64 task_docid,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mStatus(eStop),
mTaskDocid(task_docid)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(task_docid, OmnApp::getRundata());
	if (!task)
	{
		OmnThrowException("Missing task");
		return;
	}
	
	mRunNum = 0;
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "TaskThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosTaskThrd::~AosTaskThrd()
{
	//OmnScreen << "task thrd deleted" << endl;
}

bool
AosTaskThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, OmnApp::getRundata());
	aos_assert_r(task, false);

	AosRundataPtr rdata = task->getRundata();
	mStatus = eStart;

	sgTotalLock.lock();
	sgTotalTasksProcessed++;
	sgTotalLock.unlock();

	state = OmnThrdStatus::eExit;	
	bool rslt = task->startTask(rdata);

	task = 0;
	return rslt;
}

bool    
AosTaskThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, OmnApp::getRundata());
	aos_assert_r(task, false);
	aos_assert_r(mStatus != eStop, false);
	if (isFinished()) return true;

	mLock->lock();
	int progress = task->getProgress();
	OmnString crt_time = OmnGetTime(AosLocale::eChina);
	if (progress <= mProgress)
	{
		OmnAlarm << "Task thread not progressed. Previous: " << mProgress << "; "
			<< "checked at the time: " << mCheckedTime
			<< " Current: " << progress
			<< ", at the time: " << crt_time << enderr;
	}

	((AosTaskThrd*)this)->mProgress = progress;
	((AosTaskThrd*)this)->mCheckedTime = crt_time;
	mLock->unlock();
	return true;
}


void
AosTaskThrd::heartbeat(const int tid)
{
}


bool    
AosTaskThrd::signal(const int threadLogicId)
{
	return true;
}


void
AosTaskThrd::clear()
{
	mThread = 0;
}

void
AosTaskThrd::registerTaskThrd()
{
	sgTaskThrdMap[mTaskDocid] = this;
}

AosTaskThrd*
AosTaskThrd::findTaskThrd(u64 taskDocId)
{
	OmnTagFuncInfo << "taskthread map size is: " 
		<< sgTaskThrdMap.size() << endl;

	map<u64, AosTaskThrd*>::iterator itr = sgTaskThrdMap.find(taskDocId);
	if (itr != sgTaskThrdMap.end())
		return itr->second;

	return NULL;
}
