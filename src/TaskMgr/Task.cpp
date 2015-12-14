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
#include "TaskMgr/Task.h"

#include "JobTrans/UpdateTaskStatusTrans.h"
#include "JobTrans/NotifyJobTaskStopTrans.h"
#include "JobTrans/UpdateJobProcNumTrans.h"
#include "JobTrans/UpdateTaskProgressTrans.h"
#include "JobTrans/UpdateTaskOutPutTrans.h"
#include "JobTrans/UpdateTaskSnapShotsTrans.h"
#include "TaskUtil/Ptrs.h"
#include "TransClient/Ptrs.h" 
#include "JobTrans/NotifyJobTaskStopTrans.h" 
#include "JobTrans/GetRunTimeValueTrans.h" 
#include "JobTrans/SetRunTimeValueTrans.h" 
#include "SEInterfaces/JobMgrObj.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/TaskLogKeys.h"
#include "TaskMgr/TaskMgr.h"
#include "TaskMgr/DocidAllocator.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadPool.h"

AosTaskObjPtr AosTask::mTask = NULL;
TaskLog TaskLog::smTaskLog;

void
TaskLog::getLogStr(
		const u64 task_id,  
		std::ostringstream &os)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	AosTaskObjPtr task = AosTask::getTaskStatic(task_id, rdata);
	u64 job_id = task->getJobDocid();

	struct  timeval    tv;     
	struct  timezone   tz; 
	gettimeofday(&tv, &tz);

	struct tm result;                                                    
	time_t time = tv.tv_sec;                                           
	localtime_r(&time, &result);
	char time_str[25];
	sprintf(time_str, "%02d%02d-%02d:%02d:%02d.%06ld", result.tm_mon+1, result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec, tv.tv_usec);
	os << time_str << " JobId:" << job_id << " TaskId:" << task_id << " PhyId:" << AosGetSelfServerId();
}

void
TaskLog::log(const std::ostringstream &data)
{
	OmnScreen << data.str() << endl;
}

AosTask::AosTask(
		const AosTaskType::E type, 
		const OmnString &name,
		const bool flag)
:
mDocidLock(OmnNew OmnMutex()),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mTaskType(type),
mName(name),
mStatus(AosTaskStatus::eStop),
mErrorType(AosTaskErrorType::eCodeError),
mPhysicalId(-1),
mProgress(0),
mJobPublic(false),
mIsDeleteFile(false),
mTaskDocid(0),
mJobDocid(0),
mStartStampTime(0),
mVersion(0),
mMaxNumThread(0),
mTaskDataFinished(false),
mTaskRunFinished(false)
{
	if (flag)
	{
		AosTaskObjPtr taskPtr(this, false);
		registerTask(type, name, taskPtr);
	}

	mStartedNum = 0;
	mFinishedNum = 0;
	mFailedNum = 0;
}


AosTask::~AosTask()
{
}
	
void
AosTask::setTaskId(const OmnString &taskid, const AosRundataPtr &rdata)
{
	mTaskId = taskid;
}

bool
AosTask::logTask(const OmnString &logkey, const AosRundataPtr &rdata) const
{
	OmnString docstr;
	docstr << "<tasklog "
		   << AOSTAG_TASKID << "=\"" << mTaskId << "\" "
		   << AOSTAG_PARENTC << "=\"" << mLogContainer << "\" "
		   << AOSTAG_LOG_TYPE << "=\"" << AOSOTYPE_TASKLOG << "\" "
		   << AOSTAG_LOG_ENTRY_KEY << "=\"" << logkey << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" />";
	return AosAddContainerLog(mLogContainer, docstr, rdata);
}


bool
AosTask::updateTaskStatus(const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosUpdateTaskStatusTrans(mStatus, mErrorType, mErrorMsg, mTaskDocid, AosGetSelfServerId());
	aos_assert_r(trans, false);
	AosBuffPtr resp;
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);
	aos_assert_r(!timeout, false);
	aos_assert_r(resp && resp->dataLen() >= 0, false);
	return resp->getU8(0);
}


OmnString
AosTask::getTaskENV(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	aos_assert_r(key != "", "");
	mLock->lock();
	OmnString value;
	map<OmnString, OmnString>::iterator itr = mTaskENV.find(key);
	if(itr != mTaskENV.end())
	{
		value = itr->second;
	}
	mLock->unlock();
	return value;
}


bool
AosTask::initOutPut(const AosRundataPtr &rdata)
{
	AosXmlTagPtr task_doc = AosGetDocByDocid(mTaskDocid, rdata);
	aos_assert_r(task_doc, false);
	AosXmlTagPtr tags = task_doc->getFirstChild("output_data__n");
	if (!tags)
	{
		return true;
	}
	
	OmnTagFuncInfo << "taskdata output for taskdoc: " << mTaskDocid << endl; 
	AosTaskDataObjPtr task_data;
	AosXmlTagPtr subTag = tags->getFirstChild(true);
	while(subTag)
	{
		OmnString key;
		OmnString type;
		task_data = AosTaskData::serializeFromStatic(subTag, rdata);
		aos_assert_r(task_data, false);
		AosTaskDataType::E taskDataType = task_data->getTaskDataType();

		if (taskDataType == AosTaskDataType::eOutPutIIL)
		{
			OmnTagFuncInfo << "init iil output for task " << mTaskDocid << endl;
			key << subTag->getAttrStr(AOSTAG_TYPE) << "__" 
				<< subTag->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
		}
		if (taskDataType == AosTaskDataType::eOutPutDoc)
		{
			OmnTagFuncInfo << "init doc output for task " << mTaskDocid << endl;
			key << subTag->getAttrStr(AOSTAG_TYPE) << "__" 
				<< subTag->getAttrStr(AOSTAG_KEY) << "__" 
				<< subTag->getAttrStr(AOSTAG_SERVERID);
		}
		if (taskDataType == AosTaskDataType::eOutPutDocid)
		{
			OmnTagFuncInfo << "init docid output for task " << mTaskDocid << endl;
			key << subTag->getAttrStr(AOSTAG_TYPE) << "__" 
				<< subTag->getAttrStr(AOSTAG_KEY);
		}
OmnScreen << "initOutPut----------taskid : " << mTaskId << " ,========================== key : [" << key << "]" << endl;
		mTaskOutPutMap.insert(make_pair(key, task_data));
		subTag = tags->getNextChild();
	}

	//Jozhi this snapshot tag may be not save to the task doc 
	/*
	//init snapshots
	tags = task_doc->getFirstChild("snapshots__n");
	if (!tags)
	{
		return true;
	}
	subTag = tags->getFirstChild(true);
	while(subTag)
	{
		OmnString key;
		OmnString type;
		task_data = AosTaskData::serializeFromStatic(subTag, rdata);
		aos_assert_r(task_data, false);
		AosTaskDataType::E taskDataType = task_data->getTaskDataType();

		if (taskDataType == AosTaskDataType::eIILSnapShot)
		{
			key = task_data->getStrKey(rdata);
		}
		if (taskDataType == AosTaskDataType::eDocSnapShot)
		{
			key = task_data->getStrKey(rdata);
		}
OmnScreen << "initSnapShot----------taskid : " << mTaskId << " ,========================== key : [" << key << "]" << endl;

		mSnapShots[key] = task_data;
		subTag = tags->getNextChild();
	}
	*/

	return true;
}

/*
AosTaskDataObjPtr
AosTask::getSnapShot(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	map<OmnString, AosTaskDataObjPtr>::iterator itr = mSnapShots.find(key);
	if (itr != mSnapShots.end())
	{
		AosTaskDataObjPtr snapshot = itr->second;
		mSnapShots.erase(itr);
		OmnTagFuncInfo << "erase a taskdata output snapshot" << endl;
		mLock->unlock();
		return snapshot;
	}
	mLock->unlock();
	return 0;
}
*/


AosTaskDataObjPtr
AosTask::getOutPut(
		const AosTaskDataType::E &type, 
		const OmnString &key, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	OmnString strType = AosTaskDataType::toStr(type);
	OmnString strkey;
	strkey << strType << "__" << key;

	multimap<OmnString, AosTaskDataObjPtr>::iterator itr = mTaskOutPutMap.find(strkey);
	if (itr != mTaskOutPutMap.end())
	{
OmnScreen << "getOutPut----------taskid : " << mTaskId << " ,========================== key : [" << strkey << "]" << endl;
OmnTagFuncInfo << "getOutPut----------taskid : " << mTaskId << " ,========================== key : [" << strkey << "]" << endl;
		AosTaskDataObjPtr output = itr->second;
		mTaskOutPutMap.erase(itr);
		mLock->unlock();
		aos_assert_r(output, 0);
		return output;
	}
	mLock->unlock();
	return 0;
}

bool
AosTask::updateTaskSnapShots(
		vector<AosTaskDataObjPtr> &snapshots,
		const AosRundataPtr &rdata)
{
	//<taskdoc ...>
	//<snapshots__n>
	//	...
	//</snapshots__n>
	//</taskdoc>
	aos_assert_r(snapshots.size() > 0, false);
	AosXmlTagPtr snapshots_tag;
	snapshots_tag = AosXmlParser::parse("<snapshots__n/>" AosMemoryCheckerArgs);
	for (u32 i=0; i<snapshots.size(); i++)
	{
		AosXmlTagPtr snapshot_tag;
		snapshots[i]->serializeTo(snapshot_tag, rdata);
		aos_assert_r(snapshot_tag, false);
		snapshot_tag->setAttr(AOSTAG_FROM_TASK_DOCID, mTaskDocid);
		snapshots_tag->addNode(snapshot_tag);
		OmnString key = snapshots[i]->getStrKey(rdata);
		//mSnapShots[key] = snapshots[i];
OmnScreen << "===============ReduceTask createSnapShot: " << key << endl;
	}
OmnScreen << "================updateTaskStatus: " << snapshots_tag->toString() << endl;
	AosTransPtr trans = OmnNew AosUpdateTaskSnapShotsTrans(mTaskDocid, snapshots_tag, AosGetSelfServerId());
	AosBuffPtr resp;
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);
	aos_assert_r(!timeout, false);
	aos_assert_r(resp && resp->dataLen() >= 0, false);
	return resp->getU8(0);
}


bool
AosTask::updateTaskOutPut(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	//<taskdoc ...>
	//<output_data__n>
	//	...
	//<output_data__n>
	//</taskdoc>
	aos_assert_r(task_data, false);

	OmnTagFuncInfo << endl;
	AosXmlTagPtr xml;
	bool rslt = task_data->serializeTo(xml, rdata);
	aos_assert_r(rslt, false);
	xml->setAttr(AOSTAG_FROM_TASK_DOCID, mTaskDocid);
	AosTransPtr trans = OmnNew AosUpdateTaskOutPutTrans(mTaskDocid, xml, AosGetSelfServerId());
	AosBuffPtr resp;
	bool timeout = false;
	rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);
	aos_assert_r(!timeout, false);
	aos_assert_r(resp && resp->dataLen() >= 0, false);
	return resp->getU8(0);
}


bool
AosTask::actionIsFinished(const ActionInfo &action)
{
	return (action.mStatus == eFinish||
			action.mStatus == eFail ||
			action.mStatus == eAborted);
}

int
AosTask::getResource(const OmnString &res_name, const AosRundataPtr &rdata)
{
	AosXmlTagPtr task_doc = AosGetDocByDocid(mTaskDocid, rdata);
	aos_assert_r(task_doc, 1);
	return task_doc->getAttrInt(res_name, 1);
}

bool
AosTask::setTaskDataFinished()
{
	mLock->lock();
	mTaskDataFinished = true;
	mLock->unlock();
	return true;
}

bool
AosTask::taskDataIsFinished()
{
	bool finished = false;
	mLock->lock();
	finished = mTaskDataFinished;
	mLock->unlock();
	return finished;
}

bool
AosTask::startTask(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << "start task: " << mTaskDocid << endl;

	bool rslt = createSnapShot(rdata);
	aos_assert_r(rslt, false);

	rslt = taskStarted(rdata);
	aos_assert_r(rslt, false);

OmnScreen << "===========================started task: " << mTaskDocid << endl;
	rslt = initMemberAction(rdata);
	aos_assert_r(rslt, false);

	rslt = startAction(rdata);
	aos_assert_r(rslt, false);

	return true;
}

/*
bool
AosTask::commitSnapshots(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << "start to commit task snapshot" << endl;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);

	AosTaskDataObjPtr snapshot;
	AosXmlTagPtr snapshots_tag;
	AosXmlTagPtr snapshot_tag;
	u32 virtual_id = 0;
	u64 snapshot_id = 0;
	bool iscommit = false;
	bool rslt = false;
	AosTaskDataType::E type;
	AosDocType::E doctype = AosDocType::eNormalDoc;
	AosXmlTagPtr task_doc = AosGetDocByDocid(mTaskDocid, rdata);

	map<OmnString, AosTaskDataObjPtr>::iterator itr;
	itr = mSnapShots.begin();
	while(itr != mSnapShots.end())
	{
		OmnTagFuncInfo << "snapshots size is: " \
			<< mSnapShots.size() << endl;
		snapshot = itr->second;
		virtual_id = snapshot->getVirtualId();
		snapshot_id = snapshot->getSnapShotId();
		iscommit = snapshot->getIsCommit();
		if (iscommit)
		{
			itr++;
			continue;
		}

		//commit doc or iil snapshots accordingly
		type = snapshot->getTaskDataType();
		if (type == AosTaskDataType::eDocSnapShot)
		{
			OmnTagFuncInfo << "commit doc snapshot" << endl;
			doctype = snapshot->getDocType();
			rslt = doc_client->commitSnapshot(rdata, 
					virtual_id, doctype, snapshot_id, mTaskDocid);
			aos_assert_r(rslt, false);
		}
		else if (type == AosTaskDataType::eIILSnapShot)
		{
			OmnTagFuncInfo << "commit iil snapshot" << endl;
			rslt = iil_client->commitSnapshot(virtual_id, 
					snapshot_id, mTaskDocid, rdata);
			aos_assert_r(rslt, false);
		}

		//move to next snapshot
		itr++;
		OmnTagFuncInfo << "==================commiting ....: " 
			<< snapshot->getStrKey(rdata)<< endl;
	}

	OmnTagFuncInfo << "=============commiting .... save status: " 
		<< mTaskDocid << endl;
	rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_r(rslt, false);

	OmnTagFuncInfo << endl;
	mSnapShots.clear();
	return true;
}
*/

bool
AosTask::taskStarted(const AosRundataPtr &rdata)
{
	showTaskInfo(__FILE__, __LINE__, "start");

	OmnScreen << "Task Started: " << mTaskId << ":" << mTaskDocid << endl;
	setStatus(AosTaskStatus::eStart);
	return updateTaskStatus(rdata);
}


bool
AosTask::taskFinished(const AosRundataPtr &rdata)
{
	setStatus(AosTaskStatus::eFinish);
	bool is_finished = isFinished();
	aos_assert_r(is_finished, false);
	bool all_success = false;
	bool rslt = checkTaskSuccessLock(all_success);
	aos_assert_r(rslt, false);
	if (!all_success)
	{
		return taskFailed(rdata);
	}
	else
	{
		showTaskInfo(__FILE__, __LINE__, "finish");
		OmnScreen << "Task Finished: " << mTaskId << ":" << mTaskDocid << endl;
		OmnScreen << "task out put size : " << mTaskOutPutMap.size() << endl;
		return updateTaskStatus(rdata);
	}
}

bool
AosTask::taskFailed(const AosRundataPtr &rdata)
{
	AosRundataPtr tmp_rdata = rdata;
	if (!tmp_rdata)
	{
		tmp_rdata = mRundata;
	}

	OmnAlarm << "Task Failed: " << mTaskId << ":" << mTaskDocid << enderr;
	setStatus(AosTaskStatus::eFail);
	return updateTaskStatus(tmp_rdata);
}

bool
AosTask::setErrorType(const AosTaskErrorType::E &type)
{
	mErrorType = type;
	return true;
}

bool
AosTask::setErrorMsg(const OmnString &msg)
{
	mErrorMsg = msg;
	return true;
}

bool
AosTask::getNextDocids(
		const AosRundataPtr &rdata,
		u64 &docid,
		int &num_docids,
		OmnString &rcd_type_key,
		int &doc_size,
		AosXmlTagPtr &record_doc)
{
	num_docids = 0;
	
	mDocidLock->lock();
	int len = 1024; 
	AosDocidAllocatorPtr id_alloc = getDocidAllocator(rdata,
							rcd_type_key, len, record_doc);
	aos_assert_r(id_alloc, false);
	
	bool rslt = id_alloc->getNextDocids(rdata, docid, num_docids);
	mDocidLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}


AosDocidAllocatorPtr
AosTask::getDocidAllocator(
		const AosRundataPtr &rdata,
		OmnString &rcd_type_key,
		int &doc_size,
		AosXmlTagPtr &record_doc)
{
	aos_assert_r(rcd_type_key != "", 0);

	AosDocidAllocatorPtr id_alloc;
	map<OmnString, AosDocidAllocatorPtr>::iterator itr = mDocidAllocators.find(rcd_type_key);
	if(itr != mDocidAllocators.end())
	{
		id_alloc = itr->second;	
		return id_alloc;
	}

	aos_assert_r(doc_size> 0, 0);

	aos_assert_r(record_doc, 0);

	AosTaskObjPtr this_ptr(this, false);
	id_alloc = OmnNew AosDocidAllocator(rdata, this_ptr, rcd_type_key, record_doc, doc_size);	
	mDocidAllocators.insert(make_pair(rcd_type_key, id_alloc));

	return id_alloc;	
}


bool
AosTask::actionRunFinished(
		const AosRundataPtr &rdata,
		const AosActionObjPtr &action)
{
	mLock->lock();
	mTaskRunFinished = true;
	mLock->unlock();
	return true;
}

bool
AosTask::updateTaskProgress(const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosUpdateTaskProgressTrans(mTaskDocid, mProgress, AosGetSelfServerId());

	OmnTagFuncInfo << endl;
	aos_assert_r(trans, false);
	return AosSendTrans(rdata, trans);
}

bool    
AosTask::signal(const int threadLogicId)
{
	return true;
}

AosTaskObjPtr
AosTask::create()
{
	OmnNotImplementedYet;
	return 0;
}

AosTaskObjPtr	
AosTask::create(
		const AosXmlTagPtr &sdoc, 
		const AosJobInfo &jobinfo,
		const AosTaskDataObjPtr &task_data,
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

AosXmlTagPtr	
AosTask::getTaskDoc(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

bool			
AosTask::createSnapShot(const AosRundataPtr &rdata)
{
	return true;
}

bool			
AosTask::startAction(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
	
bool 			
AosTask::initMemberAction(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosXmlTagPtr 	
AosTask::getDataColTag(
		const OmnString &datacol_id, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}
	
bool			
AosTask::setDataColTags(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosValueRslt
AosTask::getRunTimeValue(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	AosValueRslt value;
	AosTransPtr trans = OmnNew AosGetRunTimeValueTrans(key, mJobDocid, mTaskDocid, AosGetSelfServerId());
	AosBuffPtr resp_buff;
	bool rslt = addReq(rdata, trans, resp_buff);
	aos_assert_r(rslt && resp_buff, value);
	return value;
}


bool				
AosTask::setRunTimeValue(
		const OmnString &key,
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosSetRunTimeValueTrans(key, value, mJobDocid, mTaskDocid, AosGetSelfServerId());
	AosBuffPtr resp_buff;
	bool rslt = addReq(rdata, trans, resp_buff);
	aos_assert_r(rslt && resp_buff, false);
	return resp_buff->getU8(0);
}


AosTaskObjPtr
AosTask::getTaskStatic(const u64 task_docid,
		const AosRundataPtr &rdata)
{
	if (!AosTask::mTask)
	{
		aos_assert_r(task_docid, NULL);
		AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);                
		aos_assert_r(task_doc, NULL);
		AosTaskObjPtr task = AosTaskObj::serializeFromStatic(task_doc, rdata);
		aos_assert_r(task, NULL);
		AosTask::mTask = task;
	}
	return AosTask::mTask;
}

void
AosTask::resetTask()
{
	AosTask::mTask = NULL;
}


// by barry for task logs
void 
AosTask::showTaskInfo(
		const char *file,
		const int line,
		const OmnString &action)
{	
	OmnString msg;
	msg << " PTSK:" << mTaskId;
	report(file, line, action, msg);
}

void
AosTask::report(
		const char *filename,
		const int lineno,
		const OmnString &action,
		const OmnString &msg)
{
	AosTaskLog(mTaskDocid) << " <" << filename << ":" << lineno << ">" << " Act:" << action << msg << endlog;
}


void
AosTask::updateCounter(
		const OmnString &str,
		const i64 counter)
{
	map<OmnString, i64>::iterator itr;
	mLock->lock();
	itr = mCounterMap.find(str);
	if (itr == mCounterMap.end())
		mCounterMap[str] = counter;
	else
		itr->second += counter;
	mLock->unlock();
}


void 
AosTask::report()
{
	map<OmnString, i64>::iterator itr;
	mLock->lock();
	for (itr = mCounterMap.begin(); itr != mCounterMap.end(); ++itr)
	{
		AosTaskLog(mTaskDocid) << " " << itr->first << ":" << itr->second << endlog;
	}
	mLock->unlock();
}
