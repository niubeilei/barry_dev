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
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskTrans/TaskTransTwoPhase.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/TaskObj.h"
#include "XmlUtil/XmlTag.h"



AosTaskTransTwoPhase::AosTaskTransTwoPhase(const bool flag)
:
AosTaskTrans(AOSTASKTRANSTYPE_TWO_PHASE, AosTaskTransType::eTwoPhase, flag)
{
	mTransType = AosTaskTransType::eTwoPhase;
	if (flag)
	{
		AosTaskTransObjPtr thisptr(this, false);
		AosTaskTransObj::registerTaskTrans(thisptr);
	}
}


AosTaskTransTwoPhase::AosTaskTransTwoPhase(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTaskTrans(AOSTASKTRANSTYPE_TWO_PHASE, AosTaskTransType::eTwoPhase, false)
{
	mTransType = AosTaskTransType::eTwoPhase;
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosTaskTransTwoPhase::~AosTaskTransTwoPhase()
{
}


bool
AosTaskTransTwoPhase::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosTaskTransTwoPhase::serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

	
bool
AosTaskTransTwoPhase::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


int
AosTaskTransTwoPhase::getProgress() const
{
	OmnNotImplementedYet;
	return -1;
}

	
OmnString
AosTaskTransTwoPhase::toString() const
{
	OmnString ss;
	ss  << "TaskTransTwoPhase: " 
		<< "\n    Type: " << mTransType
		<< "\n    Name: " << mName.data()
		<< "\n    TransId: " << mTransId
		<< "\n    Next TransId: " << mNextTransId
		<< "\n    Progress: " << mProgress
		<< "\n    Status: " << mStatus
		<< "\n    Job Objid: " << mJobObjid
		<< "\n    Job Docid: " << mJobDocid
		<< "\n    Job Server ID: " << mJobServerId;
	return ss;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::commit(vector<AosTaskObjPtr> &tasks, const AosRundataPtr &rdata)
{
	// This function commits the transaction. Since this is a two-phase commit
	// transaction, it will do the first phase commit first. If any failed
	// it may re-try. If it eventually failed the first phase, it will
	// abort the transaction. Otherwise, it starts the second phase commit. 
	mLock->lock();
	mStatus = eStatus_FirstPhase;
	tasks = mTasks;
	for (u32 i=0; i<mTasks.size(); i++)
	{
		mTasks[i]->markFirstPhaseCommit();
	}

	mLock->unlock();
	createLogEntry(task, AOSTASKTRANS_LOGKEY_FIRSTPHASE_START, rdata);	
	return AosDataProcStatus::eScheduleTasks;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskFinished(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks, 
		const AosRundataPtr &rdata)
{
	// The task 'task' finished. It logs the event. 
	aos_assert_rr(task, rdata, false);
	mLock->lock();
	tasks.clear();

	if (!taskExist(task, rdata))
	{
		mLock->unlock();
		return AosDataProcStatus::eError;
	}

	AosDataProcStatus::E rslt = AosDataProcStatus::eError;
	switch (mStatus)
	{
	case eStatus_Idle:
		 mStatus = eStatus_Active;
		 createLogEntry(task, AOSTASKTRANS_LOGKEY_TRANS_STARTED, rdata);

	case eStatus_Active:
		 rslt = taskFinishedLocked(task, tasks, rdata);
		 break;

	case eStatus_FirstPhase:
		 rslt = taskFirstPhaseFinishedLocked(task, tasks, rdata);
		 break;

	case eStatus_SecondPhase:
		 rslt = taskSecondPhaseFinishedLocked(task, tasks, rdata);
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_status") << ": " << mStatus << enderr;
		 createLogEntry(task, AOSTASKTRANS_INVALID_STATUS, rdata);
		 rslt = AosDataProcStatus::eError;
		 break;
	}

	mLock->unlock();
	return rslt;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskFailed(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks, 
		const AosRundataPtr &rdata)
{
	// The task 'task' finished. It logs the event. 
	aos_assert_rr(task, rdata, false);
	mLock->lock();
	tasks.clear();

	if (!taskExist(task, rdata))
	{
		mLock->unlock();
		return AosDataProcStatus::eError;
	}

	AosDataProcStatus::E rslt = AosDataProcStatus::eError;
	switch (mStatus)
	{
	case eStatus_Active:
		 rslt = taskFailedProtected(task, rdata);
		 break;

	case eStatus_FirstPhase:
		 rslt = taskFirstPhaseFailedLocked(task, rdata);
		 break;

	case eStatus_SecondPhase:
		 rslt = taskSecondPhaseFailedLocked(task, rdata);
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_status") << ": " << mStatus << enderr;
		 createLogEntry(task, AOSTASKTRANS_INVALID_STATUS, rdata);
		 rslt = AosDataProcStatus::eError;
		 break;
	}

	mLock->unlock();
	return rslt;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskAborted(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks, 
		const AosRundataPtr &rdata)
{
	// The task 'task' finished. It logs the event. 
	aos_assert_rr(task, rdata, false);
	mLock->lock();
	tasks.clear();

	if (!taskExist(task, rdata))
	{
		mLock->unlock();
		return AosDataProcStatus::eError;
	}

	AosDataProcStatus::E rslt = AosDataProcStatus::eError;
	switch (mStatus)
	{
	case eStatus_Active:
		 rslt = taskAbortedProtected(task, rdata);
		 break;

	case eStatus_FirstPhase:
		 rslt = taskFirstPhaseAbortedLocked(task, rdata);
		 break;

	case eStatus_SecondPhase:
		 rslt = taskSecondPhaseAbortedLocked(task, rdata);
		 break;

	default:
		 AosSetErrorU(rdata, "invalid_status") << ": " << mStatus << enderr;
		 createLogEntry(task, AOSTASKTRANS_INVALID_STATUS, rdata);
		 rslt = AosDataProcStatus::eError;
		 break;
	}

	mLock->unlock();
	return rslt;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskFinishedLocked(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	// A task has finished. It creates a log entry, 
	createLogEntry(task, AOSTASKTRANS_TASK_FINISHED, rdata);
	task->setProcFinished();
	tasks.clear();
	return AosDataProcStatus::eOk;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskFirstPhaseFinishedLocked(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	// The task finished its first phase commit. 
	createLogEntry(task, AOSTASKTRANS_TASK_FIRSTPHASE_FINISHED, rdata);
	task->setTransStatus(eStatus_FirstPhaseFinished);
	if (allFirstPhaseFinished(rdata))
	{
		// All first phase commits finished. It is the time to do the 
		// second phase commit.
		createLogEntry(task, AOSTASKTRANS_LOGKEY_FIRSTPHASE_FINISHED, rdata);	
		return startSecondPhaseCommit(tasks, rdata);
	}

	return AosDataProcStatus::eOk;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskSecondPhaseFinishedLocked(
		const AosTaskObjPtr &task, 
		const AosRundataPtr &rdata)
{
	// The task finished its second phase commit. 
	createLogEntry(task, AOSTASKTRANS_TASK_SECONDPHASE_FINISHED, rdata);
	task->setTransStatus(eStatus_SecondPhaseFinished);
	if (allSecondPhaseFinished(rdata))
	{
		// All first phase commits finished. It is the time to do the 
		// second phase commit.
		return finishTransaction(rdata);
	}

	return AosDataProcStatus::eOk;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskFirstPhaseFailedLocked(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	createLogEntry(task, AOSTASKTRANS_TASK_FIRSTPHASE_FAILED, rdata);

	if (canRetryFirstPhaseCommit(task, rdata))
	{
		// Will keep on trying
		tasks.clear();
		task.push_back(task);
		task->markFirstPhaseCommit();
		return AosDataProcStatus::eRerunTask;
	}

	// This is a serious problem. It failed first phase commit. Need to 
	// roll back all the tasks. 
	for (u32 i=0; i<mTasks.size(); i++)
	{
		mTasks[i]->markRollback();
	}

	tasks = mTasks;
	return AosDataProcStatus::eRerunTask;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskSecondPhaseFailedLocked(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	createLogEntry(task, AOSTASKTRANS_TASK_SECONDPHASE_FAILED, rdata);

	if (canRetrySecondPhaseCommit(task, rdata))
	{
		// Will keep on trying
		tasks.clear();
		tasks.push_back(task);
		task->markRetrySecondPhaseCommit();
		return AosDataProcStatus::eRerunTask;
	}

	// This is a serious problem. It failed first phase commit. Need to 
	// roll back all the tasks. 
	for (u32 i=0; i<mTasks.size(); i++)
	{
		mTasks[i]->markRollback();
	}

	tasks = mTasks;
	return AosDataProcStatus::eRerunTask;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskFirstPhaseAbortedLocked(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{

	for (u32 i=0; i<mTasks.size(); i++)
	{
		createLogEntry(mTasks[i], AOSTASKTRANS_TASK_FIRSTPHASE_ABORTED, rdata);
		mTasks[i]->markRollback();
	}

	tasks = mTasks;
	return AosDataProcStatus::eRerunTask;
}


AosDataProcStatus::E
AosTaskTransTwoPhase::taskSecondPhaseAbortedLocked(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{

	for (u32 i=0; i<mTasks.size(); i++)
	{
		mTasks[i]->markRollback();
		createLogEntry(mTasks[i], AOSTASKTRANS_TASK_SECONDPHASE_ABORTED, rdata);
	}

	tasks = mTasks;
	return AosDataProcStatus::eRerunTask;
}

