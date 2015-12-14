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
#include "TaskMgr/MapTask.h"

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


AosMapTask::AosMapTask(const bool flag)
:
AosTask(AosTaskType::eMapTask, AOSTASKNAME_MAPTASK, flag),
mCrtActSeqno(-1)
{
}


AosMapTask::~AosMapTask()
{
	//OmnScreen << "map task deleted" << endl;
}


bool
AosMapTask::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	aos_assert_r(def, false);
	aos_assert_r(mTaskData, false);

	mConfig = def;
	mTaskSdocObjid = def->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(mTaskSdocObjid != "", false);

	// 1. Create the task doc
	mPhysicalId = mTaskData->getPhysicalId();
	mDataColTags = def->getFirstChild(AOSTAG_DATA_COLLECTORS);
	mVersion = def->getAttrInt(AOSTAG_TASK_VERSION, 0);
	mMaxNumThread = def->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);

	//TaskAction
	bool rslt = initActions(def, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosMapTask::initActions(
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



AosXmlTagPtr
AosMapTask::getTaskDoc(const AosRundataPtr &rdata)
{
	// This function creates the task doc for this task. The task doc should have not
	// been created yet. Otherwise, it is an error.
	AosXmlTagPtr task_doc = 0;
	if (mTaskDocid)
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
		<< AOSTAG_TASK_SDOC_OBJID << "=\"" << mTaskSdocObjid << "\" " 
		<< AOSTAG_PARENTC << "=\"" << mTaskContainer << "\" "
		<< AOSTAG_LOG_CONTAINER << "=\"" << mLogContainer << "\" "
		<< AOSTAG_TASK_VERSION << "=\"" << mVersion << "\" "
		<< AOSTAG_MAXNUMTHREAD << "=\"" << mMaxNumThread << "\" "
		<< AOSTAG_STATUS << "=\"" << AOSTASKSTATUS_STOP << "\" "
		<< AOSTAG_START_TIME << "=\"" << mStartedTime << "\" "
		<< AOSTAG_START_STAMP << "=\"" << mStartStampTime << "\" "
		<< AOSTAG_END_TIME << "=\"" << mFinishedTime << "\" "
		<< AOSTAG_PROGRESS << "=\"0\" >";

	aos_assert_r(mTaskData, 0);

	docstr << "<" << AOSTAG_ENV << ">";
	map<OmnString, OmnString>::iterator itr = mTaskENV.begin();
	while(itr != mTaskENV.end())
	{
		docstr << "<entry zky_key=\"" << itr->first << "\">"
			   << "<![CDATA[" << itr->second << "]]>"
			   << "</entry>";
		itr++;
	}
	docstr << "</" << AOSTAG_ENV << ">";

	AosXmlTagPtr xml;
	bool rslt = mTaskData->serializeTo(xml, rdata);
	aos_assert_r(rslt && xml, 0);
	
	docstr << xml->toString();
	if (mDataColTags) docstr << mDataColTags->toString();
	docstr << "</task>";

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


bool
AosMapTask::setDataColTags(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	if (!def)
	{
		mDataColTags = 0;
		return true;
	}
	aos_assert_r(def, false);
	mDataColTags = def->clone(AosMemoryCheckerArgsBegin);
	return true;
}


AosXmlTagPtr
AosMapTask::getDataColTag(
		const OmnString &col_id, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mDataColTags, 0);

	AosXmlTagPtr tag = mDataColTags->getFirstChild(true);
	while(tag)
	{
		if (col_id == tag->getAttrStr(AOSTAG_NAME))
		{
			return tag;
		}
		tag = mDataColTags->getNextChild();
	}
	return 0;
}


void
AosMapTask::clear()
{
	mTaskData = 0;
	mDataColTags = 0;
}
	

bool
AosMapTask::startAction(const AosRundataPtr &rdata)
{
	u32 idx = 0;
	AosActionObjPtr act;
	AosTaskObjPtr thisptr(this, true);

	OmnTagFuncInfo << endl;
	while (!isFinished() && idx < mActionsInfo.size())
	{
		mLock->lock();
		mCrtActSeqno = idx;
		mLock->unlock();

		OmnString msg = "action_";
		msg << idx << ": " << AOSTASKLOG_ACTION_RUNNING;
		logTask(msg, rdata);

		act = mActionsInfo[idx].mAction;
		//act->run(thisptr, mActionsInfo[idx].mSdoc, rdata);
		act->runAction(rdata);

		while(!actionIsFinished(mActionsInfo[idx]))
		{
			mLock->lock();
			mCondVar->wait(mLock);
			mLock->unlock();
		}
		idx++;
	}


	bool rslt = taskFinished(rdata);
	aos_assert_r(rslt, false);

	for(u32 i=0; i< mActionsInfo.size(); i++)
	{
		mActionsInfo[i].mAction = 0;
	}
	mActionsInfo.clear();
	return true;
}


bool 
AosMapTask::serializeTo(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	doc->setAttr(AOSTAG_TASK_TYPE, AosTaskType::toStr(mTaskType));
	doc->setAttr(AOSTAG_TASKID, mTaskId);
	doc->setAttr(AOSTAG_LOGICID, mLogicId);
	doc->setAttr(AOSTAG_PHYSICALID, mPhysicalId);
	doc->setAttr(AOSTAG_JOB_DOCID, mJobDocid);
	doc->setAttr(AOSTAG_JOB_PUBLIC, mJobPublic);
	doc->setAttr(AOSTAG_TASK_SDOC_OBJID, mTaskSdocObjid);
	doc->setAttr(AOSTAG_PARENTC, mTaskContainer);
	doc->setAttr(AOSTAG_LOG_CONTAINER, mLogContainer);
	doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(mStatus));
	doc->setAttr(AOSTAG_START_STAMP, mStartStampTime);
	return true;
}


bool
AosMapTask::serializeFrom(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
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
	mTaskSdocObjid = doc->getAttrStr(AOSTAG_TASK_SDOC_OBJID);
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

	// the follow is ver0
	AosXmlTagPtr xml = doc->getFirstChild(AOSTAG_TASKDATA_TAGNAME);
	aos_assert_r(xml, false);
	mTaskData = AosTaskData::serializeFromStatic(xml, rdata);
	aos_assert_r(mTaskData, false);

	xml = doc->getFirstChild(AOSTAG_DATA_COLLECTORS);
	if (xml)
	{
		mDataColTags = xml->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(mDataColTags, false);
	}

	//TaskAction
	aos_assert_r(mTaskSdocObjid != "", false);                  
	AosXmlTagPtr sdoc = AosGetDocByObjid(mTaskSdocObjid, rdata);
	aos_assert_r(sdoc, false);                                  

	bool rslt = initActions(sdoc, rdata);                            
	aos_assert_r(rslt, false);                                  
	return true;
}


bool
AosMapTask::checkConfig(
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
AosMapTask::create(                                                         
		const AosXmlTagPtr &sdoc,
		const AosJobInfo &jobinfo,
		const AosTaskDataObjPtr &task_data,
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, 0);
	aos_assert_r(task_data, 0);
	aos_assert_r(logic_id != "", 0);

	OmnTagFuncInfo << endl;
	AosMapTask* task = OmnNew AosMapTask(false);  
	task->mLogicId = logic_id;
	task->mTaskData = task_data;
	task->mJobDocid = jobinfo.mJobDocid;
	task->mJobPublic = jobinfo.mJobPublic;
	task->mLogContainer = jobinfo.mLogContainer;
	task->mTaskContainer = jobinfo.mTaskContainer;
	task->mTaskENV = jobinfo.mJobENV;

	bool rslt = task->config(sdoc, rdata);
	aos_assert_r(rslt, 0);

	return task;
}


AosTaskObjPtr                                                   
AosMapTask::create()                  
{                                                               
	return OmnNew AosMapTask(false);
}                                                               


//this function will implement to logic_task
bool
AosMapTask::actionProgressed(
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
AosMapTask::actionFinished(
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
AosMapTask::actionFailed(
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
AosMapTask::checkTaskSuccessLock(bool &all_success)
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
AosMapTask::initMemberAction(const AosRundataPtr &rdata)
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
AosMapTask::onThreadInit(
		const AosRundataPtr &rdata,
		void **data)
{
	mActionsInfo[0].mAction->onThreadInit(rdata, data);
}


bool
AosMapTask::runAction(const AosRundataPtr &rdata, void *data)
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
AosMapTask::onThreadExit(
		const AosRundataPtr &rdata,
		void *data)
{
	mActionsInfo[0].mAction->onThreadExit(rdata, data);
}


bool
AosMapTask::allActionIsFinished(const AosRundataPtr &rdata)
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
AosMapTask::finishedAction(const AosRundataPtr &rdata)
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
AosMapTask::configByTaskDocVer1(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &task_doc)
{
	aos_assert_r(task_doc, false);
	
	OmnTagFuncInfo << "map task doc is: " << task_doc->toString() << endl;
	mConfig = task_doc;
	AosXmlTagPtr snapshots_tag = task_doc->getFirstChild("job_snap_shots");
	if (snapshots_tag)
	{
		//AosTaskUtil::serializeSnapShotsFrom(mJobSnapShots, snapshots_tag, rdata);
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

	// the initActions miss conf.
	mTask = this;
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
AosMapTask::configDataset(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &task_doc)
{
	mDatasets.clear();
	mDatasetsFromUpstream.clear();

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
	OmnString oldName = "";
	while(dataset_conf)
	{
		oldName = dataset_conf->getAttrStr("zky_dataset_name", "");
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
		aos_assert_r(mDatasets.count(name) == 0, false);
		OmnTagFuncInfo << "add dataset to dataset vector: " << name << endl;
		mDatasets.insert(make_pair(name, dataset));
		//If oldName has value, it should equal to name
		if (oldName != "")
			mDatasetsFromUpstream.push_back(oldName);

		//handle next input dataset
		dataset_conf = datasets_conf->getNextChild("dataset");
	}
	return true;
}


bool	
AosMapTask::createSnapShot(const AosRundataPtr &rdata)
{
	AosTaskObjPtr thisptr(this, true);
	for (u32 i=0; i<mActionsInfo.size(); i++)
	{
OmnScreen << "===============MapTask start to createSnapShot" << endl;
		bool rslt = mActionsInfo[i].mAction->createSnapShot(thisptr, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosMapTask::startTask(const AosRundataPtr &rdata)
{
	bool is_finished = isFinished();
	aos_assert_r(!is_finished, false);

	bool rslt = createSnapShot(rdata);
	aos_assert_r(rslt, false);

	rslt = taskStarted(rdata);
	aos_assert_r(rslt, false);

	rslt = initMemberAction(rdata);
	aos_assert_r(rslt, false);

OmnScreen << "===========================started task: " << mTaskDocid << endl;
	//start to run map actions
	void *data = 0;;
	void *datacheck = 0;

	onThreadInit(rdata, &data);
	if (data) datacheck = data;
	while(!isTaskRunFinished())
	{
		rslt = runAction(rdata);
		aos_assert_r(rslt, false);
	}

	aos_assert_r(data == datacheck, false);
	onThreadExit(rdata, &data);
	aos_assert_r(data==0, false);

	//gracefully finish the task run 
	finishedAction(rdata);
	taskFinished(rdata);


	return true;
}


int 
AosMapTask::getResource(const OmnString &res_name, const AosRundataPtr &rdata)
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



