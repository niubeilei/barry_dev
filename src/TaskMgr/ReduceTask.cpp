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
// 2013/04/17	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/ReduceTask.h"

#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/TaskMgr.h"
#include "TaskMgr/TaskLogKeys.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"


AosReduceTask::AosReduceTask(const bool flag)
:
AosTask(AosTaskType::eReduceTask, AOSTASKNAME_REDUCETASK, flag)
{
}


AosReduceTask::~AosReduceTask()
{
	//OmnScreen << "reduce task deleted" << endl;
}


bool
AosReduceTask::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	OmnTagFuncInfo << endl;
	mTaskSdoc = def->clone(AosMemoryCheckerArgsBegin);
	mPhysicalId = def->getAttrInt(AOSTAG_PHYSICALID, -1);

	mVersion = def->getAttrInt(AOSTAG_TASK_VERSION, 0);
	mMaxNumThread = def->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);

	bool rslt = initActions(def, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosReduceTask::initActions(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;
	AosXmlTagPtr action_tag = sdoc->getFirstChild(true);
	if (!action_tag)
	{
		AosSetErrorU(rdata, "missing_action_tag:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosActionObjPtr action = AosActionObj::getAction(action_tag, rdata);
	if (!action)
	{
		AosSetErrorU(rdata, "missing_actions:") << action_tag->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr newtag = action_tag->clone(AosMemoryCheckerArgsBegin);
	mActionInfo = ActionInfo(0, action, newtag);
	return true;
}


AosXmlTagPtr
AosReduceTask::getTaskDoc(const AosRundataPtr &rdata)
{
	AosXmlTagPtr task_doc = 0;
	if(mTaskDocid)
	{
		task_doc = AosGetDocByDocid(mTaskDocid, rdata);
		aos_assert_r(task_doc, 0);

		task_doc = task_doc->clone(AosMemoryCheckerArgsBegin);
		return task_doc;	
	}
	
	OmnString docstr;
	docstr << "<task "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASK << "\" "
		<< AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK << "\" "
		<< AOSTAG_TASK_TYPE << "=\"" << AosTaskType::toStr(mTaskType) << "\" "
		<< AOSTAG_TASKID << "=\"" << mTaskId << "\" "
		<< AOSTAG_LOGICID << "=\"" << mLogicId << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" "
		<< AOSTAG_ISPUBLIC << "=\"" << mJobPublic << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << mJobPublic << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"" << mJobPublic << "\" "
		<< AOSTAG_IS_DELETE_FILE << "=\"" << mIsDeleteFile << "\" "
		<< AOSTAG_PARENTC << "=\"" << mTaskContainer << "\" "
		<< AOSTAG_LOG_CONTAINER << "=\"" << mLogContainer << "\" "
		<< AOSTAG_TASK_VERSION << "=\"" << mVersion << "\" "
		<< AOSTAG_MAXNUMTHREAD << "=\"" << mMaxNumThread << "\" "
		<< AOSTAG_STATUS << "=\"" << AOSTASKSTATUS_STOP << "\" "
		<< AOSTAG_START_TIME << "=\"" << mStartedTime << "\" "
		<< AOSTAG_START_STAMP << "=\"" << mStartStampTime << "\" "
		<< AOSTAG_END_TIME << "=\"" << mFinishedTime << "\" "
		<< AOSTAG_PROGRESS << "=\"0\" >"
		<< mTaskSdoc->toString()
		<< "</task>";

	task_doc = AosCreateDoc(docstr, mJobPublic, rdata);
	aos_assert_r(task_doc, 0);

	task_doc = task_doc->clone(AosMemoryCheckerArgsBegin);
	mTaskDocid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	if (!mTaskDocid)
	{
		OmnAlarm << "failed to get task_docid:" << task_doc->toString() << enderr;
		return 0;
	}

	return task_doc;
}


void
AosReduceTask::clear()
{
	mTaskSdoc = 0;
}

bool
AosReduceTask::createSnapShot(const AosRundataPtr &rdata)
{
OmnScreen << "===============ReduceTask start to createSnapShot" << endl;
	AosTaskObjPtr thisptr(this, true);
	bool rslt = mActionInfo.mAction->createSnapShot(thisptr, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosReduceTask::startAction(const AosRundataPtr &rdata)
{
	OmnString msg = "action_0:";
	OmnTagFuncInfo << endl;
	msg << AOSTASKLOG_ACTION_RUNNING;
	logTask(msg, rdata);
	AosTaskObjPtr thisptr(this, true);
	mActionInfo.mAction->setTask(thisptr);
	mActionInfo.mAction->run(thisptr, mActionInfo.mSdoc, rdata);
	return true;
}


bool 
AosReduceTask::serializeTo(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	doc->setAttr(AOSTAG_TASK_TYPE, AosTaskType::toStr(mTaskType));
	doc->setAttr(AOSTAG_TASKID, mTaskId);
	doc->setAttr(AOSTAG_LOGICID, mLogicId);
	doc->setAttr(AOSTAG_PHYSICALID, mPhysicalId);
	doc->setAttr(AOSTAG_JOB_DOCID, mJobDocid);
	doc->setAttr(AOSTAG_JOB_PUBLIC, mJobPublic);
	doc->setAttr(AOSTAG_IS_DELETE_FILE, mIsDeleteFile);
	doc->setAttr(AOSTAG_PARENTC, mTaskContainer);
	doc->setAttr(AOSTAG_LOG_CONTAINER, mLogContainer);
	doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(mStatus));
	doc->setAttr(AOSTAG_START_STAMP, mStartStampTime);
	return true;
}


bool
AosReduceTask::serializeFrom(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);

	OmnTagFuncInfo << "serialize from doc is: " 
		<< doc->toString() << endl;
	mTaskType = AosTaskType::toEnum(doc->getAttrStr(AOSTAG_TASK_TYPE));
	aos_assert_r(AosTaskType::isValid(mTaskType), false);
	mTaskId = doc->getAttrStr(AOSTAG_TASKID);
	mLogicId = doc->getAttrStr(AOSTAG_LOGICID);
	mPhysicalId = doc->getAttrInt(AOSTAG_PHYSICALID, -1);
	mJobDocid = doc->getAttrU64(AOSTAG_JOB_DOCID, 0);
	mJobPublic = doc->getAttrBool(AOSTAG_JOB_PUBLIC, true);
	mIsDeleteFile = doc->getAttrBool(AOSTAG_IS_DELETE_FILE, false);
	mTaskContainer = doc->getAttrStr(AOSTAG_PARENTC);
	mLogContainer = doc->getAttrStr(AOSTAG_LOG_CONTAINER);
	mStartStampTime = doc->getAttrU64(AOSTAG_START_STAMP, 0);
	mStatus = AosTaskStatus::toEnum(doc->getAttrStr(AOSTAG_STATUS));

	mTaskDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mTaskDocid, false);
	mTaskSdoc = doc->getFirstChild("sdoc");
	aos_assert_r(mTaskSdoc, false);
	bool rslt = initActions(mTaskSdoc, rdata);                            
	aos_assert_r(rslt, false);                                  

	return true;
}


bool
AosReduceTask::checkConfig(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	return true;
}


AosTaskObjPtr
AosReduceTask::create(
		const AosXmlTagPtr &sdoc,
		const AosJobInfo &jobinfo,
		const AosTaskDataObjPtr &task_data,
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, 0);
	aos_assert_r(logic_id != "", 0);

	OmnTagFuncInfo << endl;
	AosReduceTask* task = OmnNew AosReduceTask(false);
	task->mLogicId = logic_id;
	task->mJobDocid = jobinfo.mJobDocid;
	task->mJobPublic = jobinfo.mJobPublic;
	task->mIsDeleteFile = jobinfo.mIsDeleteFile;
	task->mLogContainer = jobinfo.mLogContainer;
	task->mTaskContainer = jobinfo.mTaskContainer;

	bool rslt = task->config(sdoc, rdata);
	aos_assert_r(rslt, 0);

	return task;
}
	

AosTaskObjPtr
AosReduceTask::create()
{
	return OmnNew AosReduceTask(false);
}


bool
AosReduceTask::actionProgressed(
		const int percent,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mProgress >= percent) 
	{
		mLock->unlock();
		return true;
	}
	mProgress = percent;
	mLock->unlock();
	return updateTaskProgress(rdata);
}


bool
AosReduceTask::actionFinished(
		const AosActionObjPtr &action,
		const AosRundataPtr &rdata)
{
OmnScreen << "===========================action finished: " << mTaskDocid << endl;
	OmnString msg = "action_0:";
	msg << AOSTASKLOG_ACTION_SUCCESS;
	logTask(msg, rdata);

	mLock->lock();	
	mActionInfo.mStatus = eFinish;
	mActionInfo.mProgress = 100;
	mLock->unlock();	

	if (mVersion == 0)
	{
		OmnTagFuncInfo << endl;
		bool rslt = mActionInfo.mAction->commit(rdata);
		aos_assert_r(rslt, false);
		rslt = taskFinished(rdata);
		aos_assert_r(rslt, false);
	}
	AosTask::resetTask();
	return true;
}


bool
AosReduceTask::actionFailed(
		const AosActionObjPtr &action,
		const AosRundataPtr &rdata)
{
OmnScreen << "=====================================reduce task action failed" << mTaskDocid << endl;
	OmnString msg = "action_0:";
	msg << AOSTASKLOG_ACTION_FAILED; 
	logTask(msg, rdata);
	
	mLock->lock();	
	mErrorMsg = action->getErrorMsg();
	mActionInfo.mStatus = eFail; 
	mLock->unlock();	
	
	bool rslt = taskFinished(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosReduceTask::checkTaskSuccessLock(bool &all_success)
{
	if (mActionInfo.mStatus == eFail)
	{
		all_success = false;
		return true;
	}
	all_success = true;
	return true;
}


bool
AosReduceTask::initMemberAction(const AosRundataPtr &rdata)
{
	AosTaskObjPtr thisptr(this, true);
	OmnTagFuncInfo << endl;
	bool rslt = mActionInfo.mAction->initAction(thisptr, mActionInfo.mSdoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}


void
AosReduceTask::onThreadInit(
		const AosRundataPtr &rdata,
		void **data)
{
	mActionInfo.mAction->onThreadInit(rdata, data);
}


bool
AosReduceTask::runAction(const AosRundataPtr &rdata, void *data)
{
	bool rslt = mActionInfo.mAction->runAction(rdata, data);
	OmnTagFuncInfo << endl;
	aos_assert_r(rslt, false);
	return true;
}


void
AosReduceTask::onThreadExit(
		const AosRundataPtr &rdata,
		void *data)
{
	mActionInfo.mAction->onThreadExit(rdata, data);
}

bool
AosReduceTask::allActionIsFinished(const AosRundataPtr &rdata)
{
	while(!actionIsFinished(mActionInfo))
	{
		mLock->lock();
		mCondVar->wait(mLock);
		mLock->unlock();
	}
	return true;
}

bool
AosReduceTask::finishedAction(const AosRundataPtr &rdata)
{
	bool rslt = mActionInfo.mAction->finishedAction(rdata);
	aos_assert_r(rslt, false);

	rslt = mActionInfo.mAction->commit(rdata);
	aos_assert_r(rslt, false);
	return true;
}
