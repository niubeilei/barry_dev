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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/NormTask.h"

#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "TaskUtil/TaskUtil.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/TaskMgr.h"
#include "TaskMgr/TaskLogKeys.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"


AosNormTask::AosNormTask(const bool flag)
:
AosTask(AosTaskType::eNormTask, AOSTASKNAME_NORMTASK, flag),
mCrtActSeqno(-1)
{
}


AosNormTask::~AosNormTask()
{
	//OmnScreen << "map task deleted" << endl;
}


bool
AosNormTask::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	aos_assert_r(def, false);
	aos_assert_r(mTaskData, false);

	mConfig = def;
	mPhysicalId = mTaskData->getPhysicalId();
	mDataColTags = def->getFirstChild(AOSTAG_DATA_COLLECTORS);
	mVersion = def->getAttrInt(AOSTAG_TASK_VERSION, 0);
	mMaxNumThread = def->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);

	bool rslt = initActions(def, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosNormTask::initActions(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;
	mActionsInfo.clear();
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	if (!actions)
	{
		AosSetErrorU(rdata, "missing_actions") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u32 seqno = 0;
	AosXmlTagPtr action_tag = actions->getFirstChild(true);
	AosActionObjPtr action;
	while (action_tag)
	{
		action = AosActionObj::getAction(action_tag, rdata);
		if (!action)
		{
			AosSetErrorU(rdata, "missing_actions:") << action_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosXmlTagPtr newtag = action_tag->clone(AosMemoryCheckerArgsBegin);
		ActionInfo act(seqno, action, newtag);
		mActionsInfo.push_back(act);
		act.mAction->setTargetReporter(this);
		action_tag = actions->getNextChild();
		seqno++;
	}

	if (mActionsInfo.size() <= 0)
	{
		AosSetErrorU(rdata, "no_actions") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

void
AosNormTask::clear()
{
	mTaskData = 0;
	mDataColTags = 0;
}
	
bool 
AosNormTask::serializeTo(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	doc->setAttr(AOSTAG_TASK_TYPE, AosTaskType::toStr(mTaskType));
	doc->setAttr(AOSTAG_TASKID, mTaskId);
	doc->setAttr(AOSTAG_LOGICID, mLogicId);
	doc->setAttr(AOSTAG_PHYSICALID, mPhysicalId);
	doc->setAttr(AOSTAG_JOB_DOCID, mJobDocid);
	doc->setAttr(AOSTAG_JOB_PUBLIC, mJobPublic);
	doc->setAttr(AOSTAG_PARENTC, mTaskContainer);
	doc->setAttr(AOSTAG_LOG_CONTAINER, mLogContainer);
	doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(mStatus));
	doc->setAttr(AOSTAG_START_STAMP, mStartStampTime);
	return true;
}


bool
AosNormTask::serializeFrom(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);

	mConfig = doc;
	mTaskType = AosTaskType::toEnum(doc->getAttrStr(AOSTAG_TASK_TYPE));
	aos_assert_r(AosTaskType::isValid(mTaskType), false);

	mTaskId = doc->getAttrStr(AOSTAG_TASKID);
	mLogicId = doc->getAttrStr(AOSTAG_LOGICID);
	mPhysicalId = doc->getAttrInt(AOSTAG_PHYSICALID, -1);
	mJobDocid = doc->getAttrU64(AOSTAG_JOB_DOCID, 0);
	mJobPublic = doc->getAttrBool(AOSTAG_JOB_PUBLIC, true);
	mTaskContainer = doc->getAttrStr(AOSTAG_PARENTC);
	mLogContainer = doc->getAttrStr(AOSTAG_LOG_CONTAINER);
	mStartStampTime = doc->getAttrU64(AOSTAG_START_STAMP, 0);
	mStatus = AosTaskStatus::toEnum(doc->getAttrStr(AOSTAG_STATUS));
	mVersion = doc->getAttrInt(AOSTAG_TASK_VERSION, 0);
	mMaxNumThread = doc->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);

	AosXmlTagPtr env_tag = doc->getFirstChild(AOSTAG_ENV);
	aos_assert_r(env_tag, false);

	mTaskENV.clear();
	OmnString key, value;
	AosXmlTagPtr entry_tag = env_tag->getFirstChild(true);
	while(entry_tag)
	{
		key = entry_tag->getAttrStr(AOSTAG_KEY);
		value = entry_tag->getNodeText();
		aos_assert_r(key != "" && value != "", false);

		mTaskENV[key] = value;
		entry_tag = env_tag->getNextChild();
	}

	mTaskDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mTaskDocid, false);
	if(mVersion == 1)
	{
		// Ketty 2013/12/26
		return configByTaskDocVer1(rdata, doc);
	}
	return true;
}


bool
AosNormTask::checkConfig(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	OmnString taskSdocObjid = def->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(taskSdocObjid != "", false);

	mDataColTags = def->getFirstChild(AOSTAG_DATA_COLLECTORS);

	//TaskAction
	AosXmlTagPtr actions = def->getFirstChild(AOSTAG_ACTIONS);
	if (!actions)
	{
		AosSetErrorU(rdata, "missing_actions") << ": " << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr action_tag = actions->getFirstChild(true);
	aos_assert_r(action_tag, false);

	AosTaskObjPtr thisptr(this, true);
	AosActionObjPtr action;
	while (action_tag)
	{
		action = AosActionObj::getAction(action_tag, rdata);
		if (!action)
		{
			AosSetErrorU(rdata, "missing_actions:") << action_tag->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		bool rslt = action->checkConfig(action_tag, thisptr, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata, "maptask_action config error") << action_tag->toString() << enderr;
			return false;
		}

		action_tag = actions->getNextChild();
	}

	return true;
}
	

AosTaskObjPtr                                                   
AosNormTask::create()                  
{                                                               
	return OmnNew AosNormTask(false);
}                                                               


//this function will implement to logic_task
bool
AosNormTask::actionProgressed(
		const int percent,
		const AosRundataPtr &rdata)
{
	if (mCrtActSeqno == -1)
	{
		AosSetErrorU(rdata, "missing_crt_actions");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString msg = "action_";
	msg << mActionsInfo[mCrtActSeqno].mSeqno << ": " 
		<< AOSTASKLOG_ACTION_PROCESS << " " << percent << "%";
	logTask(msg, rdata);

	mLock->lock();	
	mActionsInfo[mCrtActSeqno].mStatus = eStart;
	mActionsInfo[mCrtActSeqno].mProgress = percent;

	int task_percent = ((mCrtActSeqno * 100) + percent) /  mActionsInfo.size();
	if (mProgress >= task_percent) 
	{
		mLock->unlock();
		return true;
	}
	mProgress = task_percent;
	mLock->unlock();
	return updateTaskProgress(rdata);
}


bool
AosNormTask::actionFinished(
		const AosActionObjPtr &action,
		const AosRundataPtr &rdata)
{
	if (mCrtActSeqno == -1)
	{
		AosSetErrorU(rdata, "missing_crt_actions");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString msg = "action_";
	msg << mActionsInfo[mCrtActSeqno].mSeqno << ": " << AOSTASKLOG_ACTION_SUCCESS;
	logTask(msg, rdata);
	
	mLock->lock();	
	mActionsInfo[mCrtActSeqno].mStatus = eFinish;
	mActionsInfo[mCrtActSeqno].mProgress = 100;
	mCondVar->signal(); 
	mLock->unlock();	
	return true;
}


bool
AosNormTask::actionFailed(
		const AosActionObjPtr &action,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mCrtActSeqno != -1, false);

	OmnString msg = "action_";
	msg << mActionsInfo[mCrtActSeqno].mSeqno << ": " << AOSTASKLOG_ACTION_FAILED; 
	logTask(msg, rdata);
	
	mLock->lock();	
	mErrorMsg = rdata->getErrmsg();
	mActionsInfo[mCrtActSeqno].mStatus = eFail; 
	mCondVar->signal(); 
	mLock->unlock();	
	
	return true;
}


bool
AosNormTask::checkTaskSuccessLock(bool &all_success)
{
	for (u32 i=0; i<mActionsInfo.size(); i++)
	{
		if (mActionsInfo[i].mStatus == eFail)
		{
			all_success = false;
			return true;
		}
	}
	all_success = true;
	return true;
}


bool
AosNormTask::initMemberAction(const AosRundataPtr &rdata)
{
	bool rslt = true;
	AosTaskObjPtr thisptr(this, true);

	OmnTagFuncInfo << endl;
	for (u32 i=0; i<mActionsInfo.size(); i++)
	{
		AosXmlTagPtr sdoc = mActionsInfo[i].mSdoc;
		rslt = mActionsInfo[i].mAction->initAction(thisptr, sdoc, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


void
AosNormTask::onThreadInit(
		const AosRundataPtr &rdata,
		void **data)
{
	mActionsInfo[0].mAction->onThreadInit(rdata, data);
}


bool
AosNormTask::runAction(const AosRundataPtr &rdata, void *data)
{
	AosTaskObjPtr thisptr(this, true);
	mCrtActSeqno = 0;
	bool rslt = false;
	OmnTagFuncInfo << "action size is: " << mActionsInfo.size() << endl;
	for (u32 i=0; i<mActionsInfo.size(); i++)
	{
		rslt = mActionsInfo[i].mAction->runAction(rdata, data);
		aos_assert_r(rslt, false);
	}
	return true;
}


void
AosNormTask::onThreadExit(
		const AosRundataPtr &rdata,
		void *data)
{
	mActionsInfo[0].mAction->onThreadExit(rdata, data);
}


bool
AosNormTask::allActionIsFinished(const AosRundataPtr &rdata)
{
	bool finished = true;
	u32 i=0;
	for (; i<mActionsInfo.size();)
	{
		finished = actionIsFinished(mActionsInfo[i]);
		if (!finished)
		{
			mLock->lock();
			mCondVar->wait(mLock);
			mLock->unlock();
			i = 0;
		}
		else
		{
			i++;
		}
	}
	return true;
}

bool
AosNormTask::finishedAction(const AosRundataPtr &rdata)
{
	for (u32 i=0; i<mActionsInfo.size(); i++)
	{
		bool rslt = mActionsInfo[i].mAction->finishedAction(rdata);
		aos_assert_r(rslt, false);

		rslt = mActionsInfo[i].mAction->commit(rdata);
	}
	return true;
}


bool
AosNormTask::configByTaskDocVer1(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &task_doc)
{
	aos_assert_r(task_doc, false);
	mConfig = task_doc;
	AosXmlTagPtr snapshots_tag = task_doc->getFirstChild("job_snap_shots");
	if (snapshots_tag)
	{
		//AosTaskUtil::serializeSnapShotsFrom(mJobSnapShots, snapshots_tag, rdata);
		//Jozhi internal iil snapshot
		AosTaskDataType::E type;
		AosTaskDataObjPtr snapshot;
		u32 virtual_id = 0;
		AosXmlTagPtr snapshot_tag = snapshot_tag->getFirstChild();
		while(snapshot_tag)
		{
			snapshot = AosTaskData::serializeFromStatic(snapshot_tag, rdata);
OmnScreen << "initSnapShots, key: " << snapshot->getStrKey(rdata) << endl;
			type = snapshot->getTaskDataType();
			if (type == AosTaskDataType::eIILSnapShot)
			{
				mIILSnapShots[virtual_id] = snapshot;
			}
			//else if (type == AosTaskDataType::eDocSnapShot)
			//{
			//	mDocSnapShots[virtual_id] = snapshot;
			//}
			snapshot_tag = snapshot_tag->getNextChild();
		}
	}

	bool rslt = configDataset(rdata, task_doc);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr xml = task_doc->getFirstChild(AOSTAG_DATA_COLLECTORS);
	if (xml)
	{
		mDataColTags = xml->clone(AosMemoryCheckerArgsBegin);
	}
	
	rslt = initActions(task_doc, rdata);
	aos_assert_r(rslt, false);

	AosTask::mTask = this;
	for (u32 i=0; i<mActionsInfo.size(); i++)
	{
		AosXmlTagPtr action_conf = mActionsInfo[i].mSdoc;
		aos_assert_r(action_conf, false);
		if(action_conf->isRootTag())	action_conf = action_conf->getFirstChild();
		rslt = mActionsInfo[i].mAction->config(mTaskDocid, action_conf, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

	
bool
AosNormTask::configDataset(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &task_doc)
{
	mDatasets.clear();
	aos_assert_r(task_doc, false);
	AosXmlTagPtr datasets_conf = task_doc->getFirstChild("input_datasets");
	OmnTagFuncInfo << endl;
	if(!datasets_conf)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return false; 
	}
	AosTaskObjPtr this_ptr(this, false);
	AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild("dataset");
	int index = 0;
	while(dataset_conf)
	{
		dataset_conf = AosDatasetObj::getDatasetConfig(rdata, dataset_conf);
		aos_assert_r(dataset_conf, false);

		OmnString name = dataset_conf->getAttrStr(AOSTAG_NAME);
		AosDatasetObjPtr dataset = AosCreateDataset(rdata.getPtr(), dataset_conf);
		if (!dataset)
		{
			AosSetError(rdata, "internal_error") << enderr;
			return false; 
		}
		dataset->setTaskDocid(mTaskDocid);
		dataset->config(rdata, dataset_conf);
		dataset->setTargetReporter(this);
		aos_assert_r(mDatasets.count(name) == 0, false);
		OmnTagFuncInfo << "add dataset to dataset vector: " << name << endl;
		mDatasets.insert(make_pair(name, dataset));
		mIndexs.insert(make_pair(index, name));
		dataset_conf = datasets_conf->getNextChild("dataset");
		index ++;
	}
	return true;
}


AosXmlTagPtr 
AosNormTask::getDataColTags()
{
	OmnString datacols_str;
	datacols_str << "<" << AOSTAG_DATA_COLLECTORS << ">";
	AosXmlTagPtr output_datasets = mConfig->getFirstChild("output_datasets");
	AosXmlTagPtr dataset_tag = output_datasets->getFirstChild(true);
	while(dataset_tag)
	{
		AosXmlTagPtr datacol_tag = dataset_tag->getFirstChild("datacollector");
		aos_assert_r(datacol_tag, 0);
		datacols_str << datacol_tag->toString();
		dataset_tag = output_datasets->getNextChild();
	}
	datacols_str << "</" << AOSTAG_DATA_COLLECTORS << ">";
	AosXmlTagPtr datacols_tag = AosXmlParser::parse(datacols_str AosMemoryCheckerArgs);
	return datacols_tag;
}


int
AosNormTask::getResource(const OmnString &res_name, const AosRundataPtr &rdata)
{
	if (res_name == AOSTAG_MAXNUMTHREAD)
	{
		if (mActionsInfo[0].mAction->getMaxThreads() > 1)
		{
			AosXmlTagPtr task_doc = AosGetDocByDocid(mTaskDocid, rdata);
			aos_assert_r(task_doc, 1);
			return task_doc->getAttrInt(res_name, 1);
		}
		else 
		{
			return 1;
		}
	}
	else 
	{
		AosXmlTagPtr task_doc = AosGetDocByDocid(mTaskDocid, rdata);
		aos_assert_r(task_doc, 1);
		return task_doc->getAttrInt(res_name, 1);
	}
}
