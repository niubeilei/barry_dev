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
// 07/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskUtil/LogicTask.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "DataCollector/DataCollector.h"
#include "DataCollector/DataCollectorSort.h"
#include "TaskUtil/Recycle.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskUtil.h"
#include "TaskMgr/TaskLogKeys.h"
#include "Job/Job.h"
#include "Job/JobLogKeys.h"
#include "Job/JobMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "TaskMgr/TaskData.h"
#include "TaskMgr/TaskMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/MapTaskCreator.h"
#include "TaskUtil/LogicMapTask.h"
#include "TaskUtil/LogicTaskVer1.h"
#include "Debug/Debug.h"

static bool mShowLog = true;

AosLogicTask::AosLogicTask(
		const AosLogicTaskType::E type,
		const u64 &job_docid)
:
mType(type),
mLock(OmnNew OmnMutex()),
mLogicId(""),
mPhyId(-1),
mJobDocid(job_docid),
mStartedNum(0),
mFinishedNum(0),
mMapTaskNum(0),
mProgress(0),
mStatus(AosTaskStatus::eStop)
{
}

AosLogicTask::~AosLogicTask()
{
	if (mShowLog) OmnScreen << "erase logic id: " << mLogicId << endl;
	mTaskInfo.clear();
}

bool						
AosLogicTask::config(
			const AosXmlTagPtr &config, 
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool						
AosLogicTask::config(
			const AosJobObjPtr &job,
			const AosXmlTagPtr &config, 
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosLogicTask::isFinished() const
{
	mLock->lock();
	if (mShowLog)
	{
		OmnScreen << "mLogicid: " << mLogicId 
		  << " ,mStartedNum: " << mStartedNum 
		  << " ,mFinishedNum: " << mFinishedNum 
		  << " ,TaskInfo size: " << mTaskInfo.size() << endl;
	}

	if (mStartedNum > 0 && mStartedNum == mFinishedNum && 
		mStartedNum == (int64_t)mTaskInfo.size())
	{
		mLock->unlock();
		return true;
	}
	mLock->unlock();
	return false;
}

bool		
AosLogicTask::updateTaskProgress(
				const AosJobObjPtr &job,
				const u64 &task_docid,
				const int percent,
				const AosRundataPtr &rdata)
{
	if (mShowLog) OmnScreen << "update task progress, task_docid:" << task_docid << ";" << endl;
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	aos_assert_rl(percent >=0 && percent <= 100, mLock, false);
	task_doc->setAttr(AOSTAG_PROGRESS, percent);
	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(info, false);
	info->setUpdateEndTime(OmnGetSecond());
	info->setProgress(percent);
	rslt = updateLogicTaskProgress(rdata);
	aos_assert_r(rslt, false);
	return job->updateJobProgress(rdata);
}

bool
AosLogicTask::updateLogicTaskProgress(const AosRundataPtr &rdata)
{
	// job and task
	int percents = 0;
	map<u64, AosTaskInfoPtr>::iterator itr = mTaskInfo.begin();
	while(itr != mTaskInfo.end())
	{
		percents += itr->second->getProgress();
		itr++;
	}
	if (mTaskInfo.size() > 0) mProgress = percents / mTaskInfo.size();
	OmnScreen << "logic_task update progress, logic_id:" << mLogicId
		<< ", progress:" << mProgress
		<< ", realSize:" << mTaskInfo.size() << endl;
	return true;
}


AosTaskInfoPtr 
AosLogicTask::getTaskInfo(
			const u64 &task_docid,
			const AosRundataPtr &rdata)
{
	mLock->lock();
	map<u64, AosTaskInfoPtr>::iterator itr = mTaskInfo.find(task_docid);
	if (itr != mTaskInfo.end())
	{
		AosTaskInfoPtr info = itr->second;
		mLock->unlock();
		return info;
	}
	mLock->unlock();
	return 0;
}

AosTaskInfoPtr
AosLogicTask::getTaskInfoLocked(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	map<u64, AosTaskInfoPtr>::iterator itr = mTaskInfo.find(task_docid);
	if (itr != mTaskInfo.end())
	{
		AosTaskInfoPtr info = itr->second;
		return info;
	}
	return 0;
}

bool
AosLogicTask::sanityCheck(
		const u64 &task_docid, 
		const int task_serverid,
		const AosRundataPtr &rdata)
{
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(info, false);
	aos_assert_r(task_serverid == info->getTaskServerId() , false);
	aos_assert_r(info->getStatus() == AosTaskStatus::eStart, false);
	aos_assert_r(info->getUpdateEndTime() != 0 , false);
	return true;
}

bool		
AosLogicTask::findFailedTaskBySvrId(
			const AosJobObjPtr &job,
			const int death_svr_id,
			const AosRundataPtr &rdata)
{
	// A task was detected down by the Server Proxy when this function is called. 
	// This function will find the corresponding task. 
	if (mShowLog) OmnScreen << "find fail task death serverid: " << death_svr_id << endl;
	vector<u64> task_docids;
	mLock->lock();
	for(map<u64, int>::iterator itr = mStartTasks.begin(); itr != mStartTasks.end();)
	{
		u64 task_docid = itr->first;
		sanityCheck(task_docid, itr->second, rdata);
		if (itr->second == death_svr_id) 
		{
			AosTaskUtil::stopTaskProcess(death_svr_id, task_docid, rdata);
			OmnString log_str;
			log_str << "faild task death taskdocid: " << task_docid  << " , svr_id : " << death_svr_id;
			OmnScreen << log_str << endl;
			addLog(log_str, rdata);
			task_docids.push_back(task_docid);
		}
		++itr;
	}
	for (u32 i = 0; i < task_docids.size(); i++)
	{
		mStartTasks.erase(task_docids[i]);
	}
	mLock->unlock();

	for (u32 i = 0; i < task_docids.size(); i++)
	{
		AosXmlTagPtr task_doc = AosGetDocByDocid(task_docids[i], rdata);
		aos_assert_r(task_doc, false);

		int task_svr_id = task_doc->getAttrInt(AOSTAG_TASKSERVERID, -1);
		aos_assert_r(task_svr_id >= 0, false);

		u64 task_stime = task_doc->getAttrU64(AOSTAG_START_STAMP, 0);

		OmnString str_type = AosTaskErrorType::toStr(
				AosTaskErrorType::eServerError);

		bool rslt = AosTaskUtil::modifyFailedTaskDoc(task_doc, 
				str_type, "", task_svr_id, rdata);
		aos_assert_r(rslt, false);

		rslt = job->taskFailed(task_doc, task_stime, rdata);
		aos_assert_r(rslt, false);
	}
	return true;

}

bool		
AosLogicTask::checkStartTask(
			const AosJobObjPtr &job,
			const AosRundataPtr &rdata)
{
	if (mShowLog)
	{
		OmnScreen << "logic task: " << mLogicId 
		<< " , checkStartTask size: " << mStartTasks.size() << endl;
	}
	print();

	vector<u64> task_docids;
	mLock->lock();
	u64 wait_time = AosTaskDriver::getSelf()->getHbTimerSec() * 300;
	for(map<u64, int>::iterator itr = mStartTasks.begin(); itr != mStartTasks.end();)
	{
		u64 task_docid = itr->first;
		int task_serverid = itr->second;

		AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
		aos_assert_rl(info, mLock, false);

		aos_assert_rl(task_serverid == info->getTaskServerId() , mLock, false);
		aos_assert_rl(info->getStatus() == AosTaskStatus::eStart, mLock, false);
		aos_assert_rl(info->getUpdateEndTime() != 0 , mLock, false);

		//int i = 0;
		u64 crt_sec = OmnGetSecond();
OmnScreen << "logic task: " << mLogicId << " , checkStartTask, "
		  << "crt_sec: " << crt_sec << " update end time: " << info->getUpdateEndTime() << endl;
		//if (crt_sec - info->getUpdateEndTime() > wait_time|| i) 
		if (crt_sec - info->getUpdateEndTime() > wait_time) 
		{
			AosTaskUtil::stopTaskProcess(task_serverid, task_docid, rdata);
			task_docids.push_back(task_docid);
			OmnString log_str;
			log_str << "task failed  crt_sec(" << crt_sec  << ") , update End Time (" 
				<< info->getUpdateEndTime() << ") task_docid: " << task_docid;
			OmnScreen << log_str << endl;
			addLog(log_str, rdata);
			//continue;
		}
		++itr;
	}
	for (u32 i = 0; i < task_docids.size(); i++)
	{
		mStartTasks.erase(task_docids[i]);
	}
	mLock->unlock();

	for (u32 i = 0; i < task_docids.size(); i++)
	{
		AosXmlTagPtr task_doc = AosGetDocByDocid(task_docids[i], rdata);
		aos_assert_r(task_doc, false);

		int task_svr_id = task_doc->getAttrInt(AOSTAG_TASKSERVERID, -1);
		aos_assert_r(task_svr_id >= 0, false);

		u64 task_stime = task_doc->getAttrU64(AOSTAG_START_STAMP, 0);

		OmnString str_type = AosTaskErrorType::toStr(
				AosTaskErrorType::eServerError);

		bool rslt = AosTaskUtil::modifyFailedTaskDoc(task_doc, 
				str_type, "", task_svr_id, rdata);
		aos_assert_r(rslt, false);
		//rslt = taskFailed(job, task_doc, task_stime, rdata);
		rslt = job->taskFailed(task_doc, task_stime, rdata);
		aos_assert_r(rslt, false);
	}
	return true;

}

bool		
AosLogicTask::createTask(
			const AosJobObjPtr &obj,
			const AosXmlTagPtr &sdoc, 
			const AosTaskDataObjPtr &task_data,
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}



bool 		
AosLogicTask::updateTaskStatus(
			const AosJobObjPtr &job,
			const AosTaskRunnerInfoPtr &run_info,
			const AosRundataPtr &rdata)
{
	AosLogicTaskObjPtr thisptr(this, false);
	AosTaskStatus::E st = run_info->getStatus();
	u64 task_docid = run_info->getTaskDocid();
	if (mShowLog) OmnScreen << "Update task status:" << run_info->getLogicId() << ":" << task_docid << ":" << AosTaskStatus::toStr(st) << endl;
	switch (st)
	{
	case AosTaskStatus::eFinish:
		 OmnScreen << "Task Finished Success:" << run_info->getLogicId() << ":" << task_docid << endl;
		 return thisptr->taskFinished(job, run_info, rdata);

	case AosTaskStatus::eFail:
		 OmnScreen << "Task Finished Fail:" << run_info->getLogicId() << ":" << task_docid << endl;
		 return thisptr->taskFailed(job, run_info, rdata);

	default:
		 break;
	}

	AosSetErrorU(rdata, "internal_error:") << run_info->getLogicId() << ":" << task_docid << ":" << st;
	OmnAlarm << rdata->getErrmsg() << enderr;

	job->logJob(AOSJOBLOG_INTERNAL_ERROR, rdata);
	return false;
}

bool		
AosLogicTask::taskFinished(
			const u64 &task_docid,
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosLogicTask::taskInfoFinished(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


void
AosLogicTask::addStartNum()
{
	OmnNotImplementedYet;
}

bool		
AosLogicTask::taskStarted(
			const u64 &task_docid,
			const u64 &timestamp,
			const int task_serverid,
			const AosRundataPtr &rdata)
{
	aos_assert_r(task_docid, false);
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	task_doc->setAttr(AOSTAG_TASKSERVERID, task_serverid);
	task_doc->setAttr(AOSTAG_START_STAMP, timestamp);
	task_doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(AosTaskStatus::eStart));
	task_doc->setAttr(AOSTAG_START_TIME, OmnGetTime(AosLocale::eChina));
	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return taskInfoStarted(task_docid, timestamp, task_serverid, rdata);
}

bool
AosLogicTask::taskInfoStarted(
		const u64 &task_docid,
		const u64 &start_time,
		const int task_serverid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_docid, false);
	mLock->lock();
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_rl(info, mLock, "");
	info->setTaskServerId(task_serverid);
	info->setUpdateEndTime(OmnGetSecond());
	info->setStatus(AosTaskStatus::eStart);
	info->setTaskStartSTime(start_time);
	mStartedNum++;
	aos_assert_r(mStartTasks.count(task_docid) == 0, false);
	mStartTasks.insert(make_pair(task_docid, task_serverid));
	mLock->unlock();
	OmnString task_id = info->getTaskId(); 
	showLogicTaskStart(task_id);
	OmnScreen << "Task Started:" << task_id << ":" << task_docid << endl;
	return true;
}


void
AosLogicTask::print()
{
	if (!mShowLog) return;
	OmnScreen << "===========================================================" << endl;
	map<u64, AosTaskInfoPtr>::iterator itr = mTaskInfo.begin();
	for(;itr != mTaskInfo.end(); ++itr)
	{
		AosTaskInfoPtr info = itr->second;
		OmnScreen << "task docid: " << info->getTaskDocid() 
			<< ";task_type:" << AosTaskType::toStr(info->getTaskType())
			<< ";progress:" << info->getProgress() 
			<< ";sts:" << AosTaskStatus::toStr(info->getStatus()) 
			<< ";run_svrid:" << info->getTaskServerId()
			<< ";optimal_svrid:" << info->getOptimalPhyId()
			<< ";task_start_time:" << info->getTaskStartSTime()
			<< endl;
	}
	OmnScreen << "=======================End=================================" << endl;

}

bool		
AosLogicTask::stopAllTaskProcess(const AosRundataPtr &rdata)
{
	map<u64, int>::iterator itr;
	for(itr = mStartTasks.begin(); itr != mStartTasks.end(); ++itr)
	{
		u64 task_docid = itr->first;

		int serverid = itr->second;
		aos_assert_r(serverid >= 0, false);

		if (mShowLog) OmnScreen << "stop task process serverid: " << serverid << endl;
		bool rslt = AosTaskUtil::stopTaskProcess(serverid, task_docid, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

void		
AosLogicTask::setTaskInfoStatus(
			const AosTaskInfoPtr &info,
			const AosTaskStatus::E sts)
{
	info->setStatus(sts);
}

bool		
AosLogicTask::addTask(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &task_doc)
{
	aos_assert_r(task_doc, false);
	AosTaskInfoPtr task_info = AosTaskInfo::create(task_doc);
	aos_assert_r(task_info, false);

	u64 task_docid = task_info->getTaskDocid();
	aos_assert_r(task_docid, false);
	
	OmnString task_id = task_info->getTaskId();
	aos_assert_r(task_id != "", false);
	
	OmnScreen << "add task to logic task:" << task_id << ":" << task_docid << endl; 
	mLock->lock();
	map<u64, AosTaskInfoPtr>::iterator itr = mTaskInfo.find(task_docid);
	aos_assert_rl(itr == mTaskInfo.end(), mLock, false);
	mTaskInfo.insert(make_pair(task_docid, task_info));
	if (task_info->getTaskType() == AosTaskType::eMapTask)
	{
		mMapTaskNum++;
	}
	mLock->unlock();
	return true;
}

bool
AosLogicTask::addTaskLocked(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &task_doc)
{
	aos_assert_r(task_doc, false);
	AosTaskInfoPtr task_info = AosTaskInfo::create(task_doc);
	aos_assert_r(task_info, false);

	u64 task_docid = task_info->getTaskDocid();
	aos_assert_r(task_docid, false);
	
	OmnString task_id = task_info->getTaskId();
	aos_assert_r(task_id != "", false);
	
	OmnScreen << "add task to logic task:" << task_id << ":" << task_docid << endl; 
	map<u64, AosTaskInfoPtr>::iterator itr = mTaskInfo.find(task_docid);
	aos_assert_rl(itr == mTaskInfo.end(), mLock, false);
	mTaskInfo.insert(make_pair(task_docid, task_info));
	return true;
}

bool 
AosLogicTask::addOutput(
	const AosXmlTagPtr &output,
	const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosLogicTask::addTriggedTaskid(
		const OmnString &task_id,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool	
AosLogicTask::getOutputFiles(
	vector<AosXmlTagPtr> &files,
	const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosLogicTask::callBack(
	const AosXmlTagPtr &output,
	const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosLogicTask::finishDataCollector(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

void
AosLogicTask::setParentDataColId(const OmnString &name, const int &shuff_id)
{
	OmnNotImplementedYet;
	return ;
}

OmnString 
AosLogicTask::getDataColId()
{
	OmnNotImplementedYet;
	return "";
}


int	
AosLogicTask::getShuffleType()
{
	return -1;
}

AosXmlTagPtr 
AosLogicTask::getDataColTags() const
{
	OmnNotImplementedYet;
	return 0;
}

void 
AosLogicTask::clear()
{
	OmnNotImplementedYet;
	return ;
}

bool
AosLogicTask::reset(const AosRundataPtr& rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosLogicTask::setDataColTags(
		const AosXmlTagPtr& conf, 
		const AosRundataPtr& rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosTaskObjPtr
AosLogicTask::createTaskPriv(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job, 0);
	AosJobInfo info;
	info.mJobDocid = job->getJobDocid();
	info.mJobPublic = job->isJobPublic();
	info.mLogContainer = job->getLogContainer();
	info.mTaskContainer = job->getTaskContainer();
	info.mJobENV = job->getJobENV();
	info.mIsDeleteFile = job->getIsDeleteFile();
	AosTaskObjPtr task = AosTaskObj::createTask(sdoc, info, 0, mLogicId, rdata);  
	return task;
}

bool
AosLogicTask::addMemberTaskPriv(
		const AosJobObjPtr &job,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosTaskInfoPtr info = 0;
	AosXmlTagPtr taskdoc = 0;
	return addMemberTaskPriv(job, task, taskdoc, info, rdata);
}


bool
AosLogicTask::addMemberTaskPriv(
		const AosJobObjPtr &job,
		const AosTaskObjPtr &task,
		AosXmlTagPtr &taskdoc,
		AosTaskInfoPtr &info,
		const AosRundataPtr &rdata)
{
	// Job and Task
	aos_assert_r(task, false);

	taskdoc = task->getTaskDoc(rdata);
	aos_assert_r(taskdoc, false);
	
	u64 task_docid = task->getTaskDocid();
	aos_assert_r(task_docid, false);
	
	bool rslt = false;
	mLock->lock();
	OmnString task_id = task->getTaskId();
	if (task_id == "")
	{
		task_id = mLogicId;
		task_id << "_" << AosTaskType::toStr(task->getTaskType()) 
			<< "_" << mTaskInfo.size();
		task->setTaskId(task_id, rdata);
		taskdoc->setAttr(AOSTAG_TASKID, task_id);
		rslt = AosModifyDoc(taskdoc, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	OmnScreen << "add task to logic task:" << task_id << ":" << task_docid << endl; 
	info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_rl(!info, mLock, false);
	info = AosTaskInfo::create(task);
	aos_assert_rl(info, mLock, false);
	aos_assert_rl(mTaskInfo.find(task_docid) == mTaskInfo.end(), mLock, false);
	mTaskInfo[task_docid] = info;
	if (info->getTaskType() == AosTaskType::eMapTask)
	{
		mMapTaskNum++;
	}
	mLock->unlock();
	return true;
}


bool
AosLogicTask::findInputFrom(
		const u64 &to_task_docid,
		list<AosXmlTagPtr> &output_list,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	map<u64, AosXmlTagPtr> from_task_docs;
	list<AosXmlTagPtr>::iterator itr;
	itr = output_list.begin();
	while(itr != output_list.end())
	{
		AosXmlTagPtr block_tag = (*itr)->getFirstChild(true);
		if (block_tag)
		{
			while(block_tag)
			{
				rslt = matchOutPut(from_task_docs, to_task_docid, block_tag, rdata);
				aos_assert_r(rslt, false);
				block_tag = (*itr)->getNextChild();
			}
		}
		else
		{
			rslt = matchOutPut(from_task_docs, to_task_docid, *itr, rdata);
			aos_assert_r(rslt, false);
		}
		itr++;
	}

	map<u64, AosXmlTagPtr>::iterator f_itr;
	f_itr = from_task_docs.begin();
	while(f_itr != from_task_docs.end())
	{
		rdata->setOk();
		AosModifyDoc(f_itr->second, rdata);
		f_itr++;
	}
	return true;
}

bool
AosLogicTask::matchOutPut(
		map<u64, AosXmlTagPtr> &from_task_docs, 
		const u64 &to_task_docid,
		const AosXmlTagPtr &input,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr from_task_doc;
	u64 from_task_docid = input->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
	aos_assert_r(from_task_docid != 0, false);
	if (from_task_docs.find(from_task_docid) == from_task_docs.end())
	{
		from_task_doc = AosGetDocByDocid(from_task_docid, rdata);
		aos_assert_r(from_task_doc, false);
		from_task_docs[from_task_docid] = from_task_doc;
	}
	else
	{
		from_task_doc = from_task_docs[from_task_docid];
	}
	aos_assert_r(from_task_doc, false);
	u64 output_file_id = 0;
	u64 input_file_id = input->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(input_file_id, false);
	AosXmlTagPtr outputs = from_task_doc->getFirstChild("output_data__n");
	aos_assert_r(outputs, false);
	AosXmlTagPtr output = outputs->getFirstChild();
	while(output)
	{
		output_file_id = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		if (output_file_id == input_file_id)
		{
			//Jozhi 
			//for rerun task the input output tracer 
			//this output may be split to multiple task
			//output->setAttr(AOSTAG_TO_TASK_DOCID, to_task_docid);
			bool found = false;
			OmnString node_body;
			node_body << to_task_docid;
			AosXmlTagPtr to_task_docid_tag = output->getFirstChild();
			if (to_task_docid_tag)
			{
				while(to_task_docid_tag)
				{
					OmnString tmp = to_task_docid_tag->getNodeText();
					if (node_body == tmp)
					{
						found = true;
						break;
					}
					to_task_docid_tag = output->getNextChild();
				}
			}
			if (!found)
			{
				AosXmlTagPtr node = output->addNode1("to_task_docid");
				node->setNodeText(node_body, false);
			}
			break;
		}
		output = outputs->getNextChild();
	}
	return true;
}


AosXmlTagPtr
AosLogicTask::getTaskDoc(const AosRundataPtr &rdata)
{  
	OmnNotImplementedYet;
	return NULL;
}


bool 
AosLogicTask::isInited() const
{
	OmnNotImplementedYet;
	return false;
}


bool						
AosLogicTask::createMergeFileTask(
			const int level,
			const AosXmlTagPtr &datacol_tag, 
			const int physical_id, 
			list <AosXmlTagPtr> &info_list, 
			u64 &task_docid, 
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool						
AosLogicTask::createMergeFinishTask(
		const AosXmlTagPtr &datacol_tag, 
		const int physical_id, 
		const AosXmlTagPtr &info, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosLogicTask::createImportDocTaskPriv(
		const AosXmlTagPtr& conf,
		int phy_id,
		std::list<AosXmlTagPtr>& outputs,
		const OmnString& action_type, 
		const AosRundataPtr& rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosLogicTask::createOutputDatasetDocs(
		const AosRundataPtr& rdata,
		const AosJobObjPtr& job)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosLogicTask::createMapTasks(
		const AosRundataPtr& rdata,
		const AosJobObjPtr& job)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosLogicTask::createReduceTasks(
		const AosRundataPtr& rdata,
		const AosJobObjPtr& job)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosLogicTask::taskFinished(
		const AosJobObjPtr& job,
		const AosTaskRunnerInfoPtr& info,
		const AosRundataPtr& rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosLogicTask::taskFailed(
		const AosJobObjPtr& job,
		const AosTaskRunnerInfoPtr& run_info,
		const AosRundataPtr& rdata)
{
	u64 task_stime = run_info->getTaskStartSTime();
	u64 task_docid = run_info->getTaskDocid();
	aos_assert_r(run_info->getStatus() == AosTaskStatus::eFail, false);

	if (!job->isValid(task_docid, task_stime, rdata)) return true;

	OmnString str_type = AosTaskErrorType::toStr(run_info->getErrorType());
	aos_assert_r(str_type != "", false);

	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	bool rslt = AosTaskUtil::modifyFailedTaskDoc(task_doc, 
			str_type, run_info->getErrmsg(), 
			run_info->getTaskRunServerId(), rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return job->taskFailed(task_doc, task_stime, rdata);
	//return taskFailed(job, task_doc, task_stime, rdata);
}


AosLogicTaskObjPtr
AosLogicTask::createLogicTask(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &config,
		const AosRundataPtr &rdata)
{
	//config->setAttr("type", "ver1");
	bool rslt = false;
	u64 job_docid = job->getJobDocid();
	AosLogicTaskObjPtr logic_task;
	AosLogicTaskType::E type = AosLogicTaskType::toEnum(config->getAttrStr("type"));
	switch(type)
	{
	case AosLogicTaskType::eVer1:
		{
			OmnNotImplementedYet;
			//logic_task = OmnNew AosLogicTaskVer1(job_docid);
			//rslt = logic_task->config(config, rdata);
			//aos_assert_r(rslt, NULL);
			//break;
		}
	case AosLogicTaskType::eMap:
		{
			logic_task = OmnNew AosLogicMapTask(job_docid);
			rslt = logic_task->config(job, config, rdata);
			aos_assert_r(rslt, NULL);
			break;
		}
	case AosLogicTaskType::eReduce:
		{
			logic_task = OmnNew AosDataCollectorSort(job_docid);
			rslt = logic_task->config(config, rdata);
			aos_assert_r(rslt, NULL);
			break;
		}
	default:
		OmnAlarm << "error type" << enderr;
		return 0;
	}

	return logic_task;
}


bool
AosLogicTask::addLog(
		const OmnString &log_str,
		const AosRundataPtr &rdata)
{
	//AosAddContainerLog(mLogContainerObjid, docstr, rdata);
	return true;
}

bool
AosLogicTask::updateJobSnapShotPriv(
		const u64 &task_docid,
		const AosXmlTagPtr &snapshots,
		const AosRundataPtr &rdata)
{
	//Jozhi 2014-07-31 update snapshot and merge to job snapshot
	aos_assert_r(snapshots, false);
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	u64 job_docid = task_doc->getAttrU64(AOSTAG_JOB_DOCID, 0);
	aos_assert_rl(job_docid != 0, mLock, false);
	mLock->unlock();
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(job_docid, rdata);
	aos_assert_r(job, false);
	return job->updateSnapShots(snapshots, rdata);
}


bool
AosLogicTask::removeStartTask(const AosTaskInfoPtr &info)
{
	u64 task_docid = info->getTaskDocid();
	mLock->lock();
	if (info->getStatus() == AosTaskStatus::eFinish)
	{
		mFinishedNum--;
		mStartedNum--;
	}
	else if (info->getStatus() == AosTaskStatus::eStart)
	{
		mStartedNum--;
	}
	else if (info->getStatus() == AosTaskStatus::eFail)
	{
		mStartedNum--;
	}


	if (mStartTasks.find(task_docid) != mStartTasks.end())
	{
		mStartTasks.erase(info->getTaskDocid());
	}
	mStatus = AosTaskStatus::eStart;
	mLock->unlock();
	return true;
}

bool
AosLogicTask::clearTaskInfo(const AosTaskInfoPtr &info)
{
	u64 task_docid = info->getTaskDocid();
	mLock->lock();
	if (mTaskInfo.find(task_docid) != mTaskInfo.end())
	{
		mTaskInfo.erase(info->getTaskDocid());
	}
	mLock->unlock();
	return true;
}

bool 
AosLogicTask::cleanDataCol(
			const u64 &task_docid,
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 	
AosLogicTask::eraseActiveTask(const AosXmlTagPtr &task_doc)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosLogicTask::reStartLogicTask(
				const AosJobObjPtr &job, 
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosLogicTask::reStartDataCol(
				const AosJobObjPtr &job,
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool		
AosLogicTask::createTask(
			const AosJobObjPtr &job,
			const AosXmlTagPtr &task_doc,
			const AosJobMgrObj::OprId oprid,
			const AosRundataPtr &rdata)
{
OmnScreen << "jozhi =============================create task by task_doc" << endl;
	// job
	bool rslt = addTask(rdata, task_doc);
	aos_assert_r(rslt, false);
	u64 task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(info && task_doc, false);
	rslt = recoverLogicTaskMemberVar(job, task_doc, info, oprid, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLogicTask::recoverLogicTaskMemberVar(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &task_doc,
		const AosTaskInfoPtr &info,
		const AosJobMgrObj::OprId oprid,
		const AosRundataPtr &rdata)
{
	switch(oprid)
	{
	case AosJobMgrObj::eNormal:
		 return true;

	case AosJobMgrObj::eReStart:
		 //Jozhi 2014-04-28 restart, snapshots will be rollback, 
		 //means cancel all the snapshots
		 break;

	case AosJobMgrObj::eSwitchJobMaster:
		 if (info->getStatus() == AosTaskStatus::eStart)
		 {
			 u64 task_docid = info->getTaskDocid();
			 int task_serverid = info->getTaskServerId();
			 job->recoverSchedulerActiveTask(task_serverid, task_docid, rdata);
			 u64 timestamp = info->getTaskStartSTime();
			 taskInfoStarted(task_docid, timestamp, task_serverid, rdata);
			 if (mShowLog) 
			 {
				 OmnScreen << "switch job master, recover start task task_docid:" << task_docid << ";task_svr_id:" << task_serverid << ";"<< endl;
			 }
		 }
		 break;

	default:
		 OmnAlarm << "miss id" << enderr;
		 return false;
	}

	if (info->getStatus() == AosTaskStatus::eFinish)
	{
		mStartedNum++;
		mFinishedNum++;
		info->setProgress(100);
		job->addRestartFinishResources(task_doc, rdata);
	}
	else
	{
		if (info->getTaskType() == AosTaskType::eReduceTask)
		{
			AosLogicTaskObjPtr thisptr(this, false);
			thisptr->insertActiveTask(task_doc);
		}
	}

	job->addRestartReduceResources(task_doc, rdata);
	return true;
}

map<u64, AosTaskInfoPtr>& 	
AosLogicTask::getAllTaskInfo() 
{
	return mTaskInfo;
}

set<OmnString>  
AosLogicTask::getDataColIds() 
{ 
	return mDataColIds; 
}


AosLogicTaskType::E			
AosLogicTask::getType() 
{ 
	return mType;
}


AosTaskStatus::E
AosLogicTask::getStatus() 
{
	return mStatus;
}	

OmnString					
AosLogicTask::getLogicId() const
{
	return mLogicId;
}

u64							
AosLogicTask::getJobDocId() const
{
	return mJobDocid; 
}


int							
AosLogicTask::getProgress() const 
{
	return mProgress;
}


void 						
AosLogicTask::setStatus(const AosTaskStatus::E &sts)
{
	mStatus = sts;
}	

bool						
AosLogicTask::merge(const AosLogicTaskObjPtr &datacol)
{
	OmnNotImplementedYet;
	return false;
}
	
bool 						
AosLogicTask::insertActiveTask(const AosXmlTagPtr &task_doc)
{
	OmnNotImplementedYet;
	return false;
}


//Jozhi for MapLogicTask
AosXmlTagPtr
AosLogicTask::createTaskDoc(
		const AosJobObjPtr &job,
		const int physical_id,
		const OmnString &task_id,
		const u32 max_num_thrd,
		const int shuffle_id,
		const OmnString &input_dataset_str,
		const OmnString &output_dataset_str,
		const AosXmlTagPtr &action_tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job, 0);
	aos_assert_r(action_tag, 0);
	OmnString env_str;
	env_str << "<" << AOSTAG_ENV << ">";
	map<OmnString, OmnString> & map_env = job->getJobENV();   
	map<OmnString, OmnString>::iterator itr = map_env.begin();
	while(itr != map_env.end())                               
	{                                                          
		env_str << "<entry zky_key=\"" << itr->first << "\">"   
				<< "<![CDATA[" << itr->second << "]]>"          
				<< "</entry>";                                  
		itr++;                                                 
	}                                                          

	if (shuffle_id > 0)
	{
		env_str << "<entry zky_key=\"" << AOSTAG_SHUFFLE_ID << "\">"   
				<< "<![CDATA[" << shuffle_id << "]]>"          
				<< "</entry>";                                  
	}
	env_str << "</" << AOSTAG_ENV << ">";

	OmnString doc_str;
	doc_str << "<task " 
		   << AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASK << "\" "
		   << AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK << "\" "
		   << AOSTAG_TASK_TYPE << "=\"" << AOSTASKNAME_NORMTASK << "\" "
		   << AOSTAG_TASKID << "=\"" << task_id << "\" "
		   << AOSTAG_LOGICID << "=\"" << mLogicId << "\" "
		   << AOSTAG_PHYSICALID << "=\"" << physical_id << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << job->getJobDocid() << "\" "
		   << AOSTAG_ISPUBLIC << "=\"" << job->isJobPublic() << "\" "
		   << AOSTAG_CTNR_PUBLIC << "=\"" << job->isJobPublic() << "\" "
		   << AOSTAG_PUBLIC_DOC << "=\"" << job->isJobPublic() << "\" "
		   << AOSTAG_IS_DELETE_FILE << "=\"" << job->getIsDeleteFile()<< "\" "
		   << AOSTAG_TASK_VERSION << "=\"" << job->getVersion() << "\" "
		   << AOSTAG_PARENTC << "=\"" << job->getTaskContainer() << "\" "
		   << AOSTAG_LOG_CONTAINER << "=\"" << job->getLogContainer() << "\" "
		   << AOSTAG_MAXNUMTHREAD << "=\"" << max_num_thrd << "\" "
		   << AOSTAG_STATUS << "=\"" << AOSTASKSTATUS_STOP << "\" "
		   //<< AOSTAG_JOBSERVERID << "=\"" << job->getJobPhysicalId() << "\" "
		   << AOSTAG_START_TIME << "=\"0\" "
		   << AOSTAG_START_STAMP << "=\"0\" "
		   << AOSTAG_END_TIME << "=\"0\" "
		   << AOSTAG_PROGRESS << "=\"0\" "
		   << ">"
		   << env_str
		   << input_dataset_str
		   << output_dataset_str
		   << "<actions>"
		   << action_tag->toString()
		   << "</actions>"
		   << "</task>";
	AosXmlTagPtr task_doc = AosCreateDoc(doc_str, job->isJobPublic(), rdata);
	return task_doc;
}


//for ReduceTask
AosXmlTagPtr
AosLogicTask::createTaskDoc(
		const AosJobObjPtr &job,
		const int physical_id,
		const u32 max_num_thrd,
		const OmnString &sdoc,
		const AosRundataPtr &rdata)
{
	OmnString task_id = mLogicId;
	task_id << "_" << AOSTASKNAME_REDUCETASK
			<< "_" << mTaskInfo.size();
	OmnString docstr;
	docstr << "<task "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_TASK << "\" "
		<< AOSTAG_STYPE << "=\"" << AOSSTYPE_TASK << "\" "
		<< AOSTAG_TASK_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		<< AOSTAG_TASKID << "=\"" << task_id << "\" "
		<< AOSTAG_LOGICID << "=\"" << mLogicId << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << physical_id << "\" "
		<< AOSTAG_JOB_DOCID << "=\"" << job->getJobDocid() << "\" "
		<< AOSTAG_ISPUBLIC << "=\"" << job->isJobPublic() << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << job->isJobPublic() << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"" << job->isJobPublic() << "\" "
		<< AOSTAG_IS_DELETE_FILE << "=\"" << job->getIsDeleteFile() << "\" "
		<< AOSTAG_TASK_VERSION << "=\"" << job->getVersion() << "\" "
		<< AOSTAG_PARENTC << "=\"" << job->getTaskContainer() << "\" "
		<< AOSTAG_LOG_CONTAINER << "=\"" << job->getLogContainer() << "\" "
		<< AOSTAG_MAXNUMTHREAD << "=\"" << max_num_thrd << "\" "
		<< AOSTAG_STATUS << "=\"" << AOSTASKSTATUS_STOP << "\" "
		//<< AOSTAG_JOBSERVERID << "=\"" << job->getJobPhysicalId() << "\" "
		<< AOSTAG_START_TIME << "=\"0\" "
		<< AOSTAG_START_STAMP << "=\"0\" "
		<< AOSTAG_END_TIME << "=\"0\" "
		<< AOSTAG_PROGRESS << "=\"0\" >"
		<< sdoc
		<< "</task>";
	AosXmlTagPtr task_doc = AosCreateDoc(docstr, job->isJobPublic(), rdata);
	aos_assert_r(task_doc, 0);
	return task_doc;
}


void
AosLogicTask::showLogicTaskStart(
		const OmnString &task_id)
{
	AosJobLog(mJobDocid) << "Act:start LTSK:" << mLogicId << " PTSK:" << task_id << endlog;
}

void 
AosLogicTask::showCreateLogicTask(
		const OmnString &task_id)
{
	AosJobLog(mJobDocid) << "Act:create LTSK:" << mLogicId << " PTSK:" << task_id << endlog;
}

void 
AosLogicTask::showLogicTaskTotalFinish()
{
	AosJobLog(mJobDocid) << "Act:finish LTSK:" << mLogicId << endlog;
}

void 
AosLogicTask::showLogicTaskFinish(
		const u64 task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert(task_docid);
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);                
	aos_assert(task_doc);
	OmnString task_id = task_doc->getAttrStr("zky_taskid", "");
	aos_assert(task_id != "");
	AosJobLog(mJobDocid) << "Act:finish LTSK:" << mLogicId << " PTSK:" << task_id << endlog;
}

