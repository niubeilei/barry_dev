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
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskTrans/TaskTrans.h"

#define AOSTASKTRANS_LOGKEY_TASK_STARTED			"task_started"
#define AOSTASKTRANS_LOGKEY_TASK_FINISHED			"task_finished"

AosTaskTrans::AosTaskTrans(
		const OmnString &name, 
		const AosTaskTransType::E type,
		const bool flag)
{
	OmnNotImplementedYet;
}


AosTaskTrans::~AosTaskTrans()
{
}


OmnString 
AosTaskTrans::statusToStr(const Status s)
{
	OmnNotImplementedYet;
	return "";
}


bool 
AosTaskTrans::taskStarted(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	// This function is called when a job starts a task. Note that 
	// a task transaction does not control how many tasks to run, but
	// if a job wants a task transaction to manage the execution of 
	// some tasks, it needs to add the tasks whenever they are ready
	// to run. The transaction guarantees that these tasks either 
	// runs to the end of their processing or do nothing.
	aos_assert_rr(task, rdata, false);
	u64 task_docid = task->getTaskDocid();
	aos_assert_rr(task_docid, rdata, false);
	mLock->lock();
	if (getTask(task_docid))
	{
		AosSetErrorU(rdata, "task_already_started") << ": " << task_docid << enderr;
		mLock->unlock();
		return false;
	}

	mTasks[task_docid] = task;
	createLogEntry(task, AOSTASKTRANS_LOGKEY_TASK_STARTED, rdata);
	mLock->unlock();
	return true;
}


AosDataProcStatus::E
AosTaskTrans::taskFailedProtected(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	// The task 'task' failed. It checks whether the task can re-run. If yes, 
	// it asks the task to re-run.
	aos_assert_rr(task, rdata, AosDataProcStatus::eError);
	createLogEntry(task, AOSTASKTRANS_LOGKEY_TASK_FAILED, rdata);
	if (!taskExist(task, rdata)) 
	{
		return AosDataProcStatus::eError;
	}

	// Check whether it can local retry
	if (canDoLocalRetry(task, rdata))
	{
		// It can do the local try. 
		task->increaseLocalTries();
		task->resetForLocalRerun(rdata);
		createLogEntry(task, AOSTASKTRANS_LOGKEY_LOCAL_RERUN, rdata);
		tasks.clear();
		tasks.push_back(task);
		return AosDataProcStatus::eRerunTask;
	}

	// Check whether it can remote retry
	if (canDoRemoteRetry(task, rdata))
	{
		task->increaseRemoteRetry();
		task->resetForRemoteRerun(rdata);
		createLogEntry(task, AOSTASKTRANS_LOGKEY_REMOTE_RERUN, rdata);
		return AosDataProcStatus::eRemoteRerunTask;
	}

	// Cannot retry anymore. This will fail the transaction, which needs
	// to send a request to all the tasks to roll back. 
	tasks = mTasks;
	mStatus = eFailed;
	createLogEntry(task, AOSTASKTRANS_LOGKEY_TRANS_FAILED, rdata);
	return AosDataProcStatus::eRollbackTasks;
}


bool
AosTaskTrans::abort(
		const AosTaskObjPtr &task, 
		vector<AosTaskObjPtr> &tasks, 
		const AosRundataPtr &rdata) 
{
	// The task 'task' is aborted. This function will roll back all the
	// tasks and fail the transaction.
	aos_assert_rr(task, rdata, AosDataProcStatus::eError);
	mLock->lock();
	if (!taskExist(task, rdata))
	{
		mLock->unlock();
		return AosDataProcStatus::eError;
	}

	mStatus = eAborted;
	tasks = mTasks;
	mLock->unlock();
	createLogEntry(task, AOSTASKTRANS_LOGKEY_TRANS_ABORTED, rdata);
	
	return AosDataProcStatus::eRollbackTasks;
}


bool
AosTaskTrans::createLogEntry(
		const AosTaskObjPtr &task, 
		const OmnString &log_key, 
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<log ";
	docstr << AOSTAG_PARENTC << "=\"" << mLogContainer
		<< "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASKTRANSLOG
		<< "\" " << AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK_STARTED
		<< "\" " << AOSTAG_LOG_ENTRY_KEY << "=\"" << logkey 
		<< "\" " << AOSTAG_JOB_OBJID << "=\"" << mJobObjid
		<< "\" " << AOSTAG_JOB_DOCID << "=\"" << mJobDocid
		<< "\" " << AOSTAG_TASK_SEQNO << "=\"" << task->getSeqno()
		<< "\" " << AOSTAG_TASK_Objid << "=\"" << task->getTaskObjid()
		<< "\" " << AOSTAG_TASK_DOCID << "=\"" << task->getTaskDocid()
		<< "\" " << AOSTAG_TASK_TRANSID << "=\"" << mTransId
		<< "\" >" 
		<< "</translog>";
	
	OmnScreen << docstr << endl;
	return AosAddLog(mLogContainer, docstr, rdata);
}


bool
AosTaskTrans::modifyTransStatus(
		const AosXmlTagPtr &transDoc, 
		const OmnString &taskStatus, 
		const AosRundataPtr &rdata)
{
	if (!transDoc)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}
		
	bool rslt = AosModifyDocAttr(transDoc, AOSTAG_STATUS, taskStatus, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_modify_task_doc") << ": " << transDoc->toString();;
		AosLogError(rdata);
		return false;
	}
	return true;
}


AosXmlTagPtr
AosTaskTrans::createTransDoc(
		const u32 task_seqno,
		const bool is_public,
		const AosRundataPtr &rdata)
{
	// This function creates the task trans doc. The doc should have not
	// been created yet. Otherwise, it is an error.

	aos_assert_rr(!mTransDoc, rdata, mTransDoc);
	OmnString docstr = "<tasktrans ";
	OmnString pubstr = (mIsPublic)?"true":"false";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASK_TRANS
		<< "\" " << AOSTAG_CTNR_PUBLIC << "=\"" << pubstr  
		<< "\" " << AOSTAG_PUBLIC_DOC << "=\"" << pubstr
		<< "\" " << AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK_TRANS
		<< "\" " << AOSTAG_TAG << "=\"" << mTransTags
		<< "\" " << AOSTAG_JOB_OBJID << "=\"" << mJobObjid
		<< "\" " << AOSTAG_JOB_DOCID << "=\"" << mJobDocid
		<< "\" " << AOSTAG_JOBSERVERID << "=\"" << mJobServerId
		<< "\" " << AOSTAG_STATUS << "=\"" << statusToStr(mStatus)
		<< "\" " << AOSTAG_FINISHED << "=\"" << "false"
		<< "\" " << AOSTAG_PARENTC << "=\"" << mTransContainer
		<< "\" " << AOSTAG_TRANS_TYPE << "=\"" << AosTaskTransType::toStr(mTransType)
		<< "\" " << AOSTAG_TASK_TRANSID << "=\"" << mTransId
		<< "\" " << AOSTAG_NEXT_TASK_TRANSID << "=\"" << mNextTransId
		<< "\" " << AOSTAG_TRANS_PROGRESS << "=\"0" 
		<< "\" >"
		<< "<description><![CDATA[" << mDescription << "]]>"
		<< "</description>"
		<< "</tasktrans>";

	try
	{
		mTransDoc = AosCreateDoc(docstr, is_public, rdata);
		aos_assert_r(mTransDoc, 0);
		return mTransDoc;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_create_task");
		AosLogError(rdata);
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


