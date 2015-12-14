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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 08/13/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/TaskMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "JobTrans/AbortTaskTrans.h"
#include "JobTrans/ResumeTaskTrans.h"
#include "JobTrans/ScheduleTaskTrans.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/JobMgrObj.h"
#include "TaskMgr/MapTask.h"
#include "TaskMgr/TaskLogKeys.h"
#include "TaskMgr/TaskThrd.h"
#include "TaskMgr/TaskScheduler.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskUtil.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadPool.h"
#include "TransClient/Ptrs.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SysMsg/StartDynamicProcMsg.h"
#include "SysMsg/KillProcMsg.h"
#include "JobTrans/StartTaskTrans.h"
#include "JobTrans/NotifyJobTaskStopTrans.h"
#include "JobTrans/GetFileListByDataCol.h"
#include "JobTrans/ClearTaskIILEntryMapTrans.h"
#include "JobTrans/CreateDataCol.h"
#include "TaskUtil/Recycle.h"
#include "TaskMgr/TaskData.h"
#include "Debug/Debug.h"
#include "JobTrans/GetRunTimeValueByJobTrans.h"
#include "JobTrans/SetRunTimeValueByJobTrans.h"

OmnSingletonImpl(AosTaskMgrSingleton,
                 AosTaskMgr,
                 AosTaskMgrSelf,
                "AosTaskMgr");

//static u32 sgMaxStartTask = AosTaskMgr::eMaxStartTask;
static u32 sgMaxStartTask = AosTaskMgr::eMaxStartTask;


AosTaskMgr::AosTaskMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mNumTask(0),
mNumProc(0),
mIsTest(false)
{
}


AosTaskMgr::~AosTaskMgr()
{
}


bool
AosTaskMgr::start()
{
	OmnTagFuncInfo << endl;

	OmnThreadedObjPtr thisPtr(this, false);
	mStartTaskThread = OmnNew OmnThread(thisPtr, "TaskMgrStartTaskThrd", 0, true, true, __FILE__, __LINE__);
	mStartTaskThread->start();
	mLock->lock();
	if (mWaitProcs.size() <= 0)
	{
		//create only 1 task at the beginning
		for (u32 i=0; i<1; i++)
		{
			OmnScreen << "======================send start task process: " << endl;
			AosAppMsgPtr msg = OmnNew AosStartDynamicProcMsg(AosProcessType::eTask);
			aos_assert_r(msg, false);
			AosSendMsg(msg);
		}
	}
	mLock->unlock();
	return true;
}


bool
AosTaskMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosTaskMgr::stop()
{
    return true;
}


bool    
AosTaskMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 logic_pid = 0;
	OmnTagFuncInfo << endl;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mWaitTasks.size() <= 0)
		{
			mCondVar->wait(mLock);
			mThreadStatus = true;
			mLock->unlock();
			continue;
		}

		if (mWaitProcs.size() <= 0)
		{
			mCondVar->wait(mLock);
			mThreadStatus = true;
			mLock->unlock();
			continue;
		}

		if (mStartTasks.size() >= sgMaxStartTask)
		{
			mCondVar->wait(mLock);
			mThreadStatus = true;
			mLock->unlock();
			continue;
		}

		while (mWaitTasks.size() > 0 && 
				mWaitProcs.size() > 0 &&
				mStartTasks.size() < sgMaxStartTask)
		{
			logic_pid = mWaitProcs.front();
			mWaitProcs.pop_front();
			TaskInfo info = mWaitTasks.front();
			info.mLogicPid = logic_pid;
			mWaitTasks.pop_front();
			startTask(info.mTaskDocid, logic_pid, info.mRundata); 
			mStartTasks[info.mTaskDocid] = info;
		}

		mLock->unlock();
	}
	return true;
}

bool    
AosTaskMgr::threadFuncInMem(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 logic_pid = 0;
	OmnTagFuncInfo << endl;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		
		//we need to seperate job and service tasks
		//and processes later on
		while (mWaitTasks.size() > 0 && mWaitProcs.size() > 0)
		{
			logic_pid = mWaitProcs.front();
			mWaitProcs.pop_front();
			TaskInfo info = mWaitTasks.front();
			info.mLogicPid = logic_pid;
			mWaitTasks.pop_front();
			startTask(info.mTaskDocid, logic_pid, info.mRundata); 
			mStartTasks[info.mTaskDocid] = info;
		}

		mLock->unlock();
		//check if there are tasks to run every 1 second
		OmnSleep(1);
	}
	return true;
}

bool    
AosTaskMgr::signal(const int threadLogicId)
{
	return true;
}

bool
AosTaskMgr::addTask(
		const int crt_jobsvrid,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	// Ken Lee, 2013/08/25
	OmnTagFuncInfo << "task docid is: " << task_docid << endl;
	int m = 10;
	AosXmlTagPtr task_doc;
	while (m--)
	{
		task_doc = AosGetDocByDocid(task_docid, rdata);
		if (task_doc) break;

		OmnSleep(3);
	}
	aos_assert_r(task_doc, false);
	int task_serverid = task_doc->getAttrInt(AOSTAG_TASKSERVERID, -1);
	aos_assert_r(task_serverid >= 0, false);

	u64 timestamp = task_doc->getAttrU64(AOSTAG_START_STAMP, 0); 
	aos_assert_r(timestamp, false);

	u64 job_docid = task_doc->getAttrU64(AOSTAG_JOB_DOCID, 0);
	aos_assert_r(job_docid, false);

	OmnString logic_id = task_doc->getAttrStr(AOSTAG_LOGICID, "");
	aos_assert_r(logic_id != "", false);

	OmnString task_type = task_doc->getAttrStr(AOSTAG_TASK_TYPE, "");
	aos_assert_r(task_type != "", false);

	AosTransPtr trans = OmnNew AosScheduleTaskTrans(job_docid, logic_id, task_type, 
			crt_jobsvrid, task_docid, task_serverid, timestamp);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosTaskMgr::addTaskLocal(
		const u64 &job_docid,
		const OmnString &logic_id,
		const OmnString &task_type,
		const int crt_jobsvrid,
		const u64 &task_docid,
		const u64 &start_stamp,
		const AosRundataPtr &rdata)
{
	OmnScreen << "=========================add Task To TaskMgr:" << task_docid << endl;
	OmnTagFuncInfo << "task docid is: " << task_docid << endl;
	mLock->lock();
	if (isFindWaitTaskLocked(task_docid))
	{
		mLock->unlock();
		return true;
	}

	if (isFindStartTaskLocked(task_docid))
	{
		OmnAlarm << "may be task has been running" << enderr;
		//the started task has not been killed
	}
	TaskInfo info;
	info.mJobDocid = job_docid;
	info.mLogicId = logic_id;
	info.mTaskType = task_type;
	info.mTaskDocid = task_docid;
	info.mStartStamp = start_stamp;
	info.mProgress = 0;
	info.mStatus = AosTaskStatus::eStop;
	info.mErrorType = AosTaskErrorType::eInvalid;
	info.mRundata = rdata;
	info.mCrtJobSvrId = crt_jobsvrid;
	mWaitTasks.push_back(info);
OmnScreen << "==================add task local: " << task_docid 
		  << " wait task size: " << mWaitTasks.size()
		  << " wait proc size: " << mWaitProcs.size()
		  << " start num task: " << mNumTask++
		  << endl;
	if (mWaitProcs.size() < mWaitTasks.size())
	{
OmnScreen << "======================send start task process: " << endl;
		AosAppMsgPtr msg = OmnNew AosStartDynamicProcMsg(AosProcessType::eTask);
		aos_assert_rl(msg, mLock, false);
		AosSendMsg(msg);
	}
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool
AosTaskMgr::stopTaskProcCb(const u32 logic_pid, const int svr_id)
{
	u64 task_docid;
	AosRundataPtr rdata;
	mLock->lock();
	getStartTaskInfoLocked(logic_pid, task_docid, rdata);
OmnScreen << "==================================stop task callback : " 
	      << " proc logic pid : " << logic_pid 
	      << " start task docid: " << task_docid
		  << endl;
	if (task_docid != 0)
	{
		OmnString error_msg = "task is down";
		mLock->unlock();
		return updateTaskStatusLocked(AosTaskStatus::eFail, AosTaskErrorType::eCoreDumpError, error_msg, logic_pid, task_docid, rdata);
	}
	else
	{
		//Jozhi 2015-30-24, if this task process has no task is running,
		//kill this task process, will remove from wait queue
OmnScreen << "jozhi remove logic_pid from wait queue: " << logic_pid << endl;
		removeWaitProcLocked(logic_pid);
	}
	mLock->unlock();
	return true;
}


bool
AosTaskMgr::startTaskProcCb(const u32 logic_pid, const int proc_pid, const int svr_id)
{
	mLock->lock();
	mWaitProcs.push_back(logic_pid);
	mProcIdMap[logic_pid] = proc_pid;
OmnScreen << "================================start task callback : " 
		  << " logic_pid: " << logic_pid 
		  << " proc pid: " << proc_pid
		  << " wait proc size: " << mWaitProcs.size()
		  << " wait task size: " << mWaitTasks.size()
		  << " start num process: " << mNumProc++
		  << endl;
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool
AosTaskMgr::isFindWaitProcLocked(const u32 logic_pid)
{
	deque<u32>::iterator itr = mWaitProcs.begin();
	while(itr != mWaitProcs.end())
	{
		if ((*itr) == logic_pid)
		{
			return true;
		}
		itr++;
	}
	return false;

}

bool
AosTaskMgr::isFindWaitTaskLocked(const u64 &task_docid)
{
	deque<TaskInfo>::iterator itr = mWaitTasks.begin();
	while(itr != mWaitTasks.end())
	{
		if ((*itr).mTaskDocid == task_docid)
		{
			return true;
		}
		itr++;
	}
	return false;
}

bool
AosTaskMgr::isFindStartTaskLocked(const u64 &task_docid)
{
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	return itr != mStartTasks.end();
}

bool
AosTaskMgr::updateTaskProcNum(
		const u64 &task_docid,
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return true;
	}
	(itr->second).mProcNum = xml;
OmnScreen << "============================updateTaskProcNum: " << task_docid << xml->toString() << endl;
	mLock->unlock();
	return true;
}

bool
AosTaskMgr::updateTaskSnapShots(
		const u64 &task_docid,
		const AosXmlTagPtr &snapshots_tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(snapshots_tag, false);
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return true;
	}
	(itr->second).mSnapShots = snapshots_tag;
OmnScreen << "============================updateTaskSnapShots: " << task_docid << " : " << snapshots_tag->toString() << endl;
	mLock->unlock();
	return true;
}

bool
AosTaskMgr::updateTaskOutPut(
		const u64 &task_docid,
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << "task_docid is: " << endl;
	aos_assert_r(xml, false);
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return true;
	}
	((itr->second).mOutPuts).push_back(xml);
OmnScreen << "============================updateTaskOutPut: " << task_docid << " : " << xml->toString() << endl;
	mLock->unlock();
	return true;
}

bool
AosTaskMgr::updateTaskProgress(
		const u64 &task_docid,
		const int percent,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << "task_docid is: " << endl;
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return true;
	}
	(itr->second).mProgress = percent;
	int job_server_id = (itr->second).mCrtJobSvrId;
	mLock->unlock();

	aos_assert_r(job_server_id != -1, false);	
	AosTransPtr trans = OmnNew AosNotifyJobTaskStopTrans(job_server_id, AosGetSelfServerId());
	bool rslt = AosSendTrans(rdata, trans);
	return rslt;
}
bool
AosTaskMgr::updateTaskStatus(
		const AosTaskStatus::E &status,
		const AosTaskErrorType::E &error_type,
		const OmnString &error_msg,
		const u32 logic_pid,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	mLock->lock();
	rslt = updateTaskStatusLocked(status, error_type, error_msg, logic_pid, task_docid, rdata);
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTaskMgr::updateTaskStatusLocked(
		const AosTaskStatus::E &status,
		const AosTaskErrorType::E &error_type,
		const OmnString &error_msg,
		const u32 logic_pid,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	OmnScreen << logic_pid << " status is :" << AosTaskStatus::toStr(status) << " task docid : " << task_docid << endl;
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		return true;
	}
	(itr->second).mStatus = status;
	(itr->second).mErrorType = error_type; 
	(itr->second).mErrmsg = error_msg; 
	int job_server_id = (itr->second).mCrtJobSvrId;
	aos_assert_r(job_server_id != -1, false);	
	AosTransPtr trans = OmnNew AosNotifyJobTaskStopTrans(job_server_id, AosGetSelfServerId());
	bool rslt = AosSendTrans(rdata, trans);
	return rslt;
}

bool 
AosTaskMgr::getDataColOutput(
		const OmnString &datacol_id,
		const u64 &job_id,
		const u64 &task_docid,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return true;
	}

	int job_server_id = (itr->second).mCrtJobSvrId;
	mLock->unlock();
	AosTransPtr trans = OmnNew AosGetFileListByDataCol(datacol_id, job_id, job_server_id);
	bool rslt = addReq(rdata, trans, buff);  
	return rslt;
}


bool 
AosTaskMgr::createDataCol(
		const OmnString &datacol_id,
		const u64 &job_id,
		const u64 &task_docid,
		const OmnString &config,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return true;
	}

	int job_server_id = (itr->second).mCrtJobSvrId;
	mLock->unlock();
	AosTransPtr trans = OmnNew AosCreateDataCol(datacol_id, job_id, job_server_id, config);
	bool rslt = addReq(rdata, trans, buff);  
	return rslt;
}



bool
AosTaskMgr::startTask(const u64 &task_docid, const u32 logic_pid, const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosStartTaskTrans(task_docid, AosGetSelfServerId(), logic_pid);
	aos_assert_r(trans, false);
OmnScreen << "============================send start task trans: " << task_docid << " : " << logic_pid  << " : " << trans->getTransId().toString() << endl;
	bool rslt = AosSendTrans(rdata, trans);

	OmnTagFuncInfo << "start a new task trans, task_docid is: " << 
		task_docid << " logic_pid is: " << logic_pid << endl;
	aos_assert_r(rslt, false);
	return true;
}

bool
AosTaskMgr::stopTask(const u64 &task_docid, const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << "task_docid is: " << endl;

	mLock->lock();
	if (isFindWaitTaskLocked(task_docid))
	{
		removeWaitTaskLocked(task_docid);
		mLock->unlock();
		return true;
	}
	if (isFindStartTaskLocked(task_docid))
	{
		u32 logic_pid = getStartTaskPidLocked(task_docid);
		removeStartTaskLocked(task_docid, true, rdata);
		realKillProcPriv(logic_pid);
	}
	mLock->unlock();
	return true;
}

bool
AosTaskMgr::realKillProcPriv(const u32 logic_pid)
{
	AosAppMsgPtr msg = OmnNew AosKillProcMsg(AosGetSelfServerId(), logic_pid);
	return AosSendMsg(msg);
}

bool
AosTaskMgr::killProcPriv(const u32 logic_pid)
{
OmnScreen << "reuse task.exe, logic_pid: " << logic_pid << endl;
	mWaitProcs.push_back(logic_pid);
	return true;
}


bool
AosTaskMgr::taskFinishedLocked(
		const u64 &task_docid, 
		const u32 logic_pid,
		const AosRundataPtr &rdata)
{
	if (isService(rdata, task_docid))
		return taskFinishedLockedInMem(task_docid, logic_pid, rdata);

	if (isFindStartTaskLocked(task_docid))
	{
		removeStartTaskLocked(task_docid, false, rdata);
		//Jozhi temp to fix the task.exe memory leak
		map<u32, int>::iterator itr = mProcIdMap.find(logic_pid);
		aos_assert_r(itr != mProcIdMap.end(), false);
		int pid = itr->second;
		double vm_mem, re_mem;
		OmnApp::getSelfMemoryUsage(pid, vm_mem, re_mem);
		//this size unit is (KB)
		//bool is_kill = (re_mem >= 10000000.0);
		bool is_kill = false;
		if (is_kill)
		{
			OmnScreen << "may be memory leak to kill task.exe, task_docid: " << task_docid
				<< " , logic_pid: " << logic_pid << " process id: " << pid << endl;
			realKillProcPriv(logic_pid);
		}
		else
		{
			killProcPriv(logic_pid);
		}
		mCondVar->signal();
	}
	return true;
}


bool
AosTaskMgr::taskStartedLocked(
		const u64 &task_docid,
		const u32 logic_pid,
		const AosRundataPtr &rdata)
{
	if (isService(rdata, task_docid))
		return taskStartedLockedInMem(task_docid, logic_pid, rdata);

OmnScreen << "==========================task started : " << task_docid << " : " << logic_pid << endl;
	return true;
}


bool
AosTaskMgr::taskFailedLocked(
		const u64 &task_docid,
		const u32 logic_pid,
		const AosRundataPtr &rdata)
{
	if (isService(rdata, task_docid))
		return taskFailedLockedInMem(task_docid, logic_pid, rdata);

	if (isFindStartTaskLocked(task_docid))
	{
		removeStartTaskLocked(task_docid, true, rdata);
		realKillProcPriv(logic_pid);
		mCondVar->signal();
	}
	return true;
}

bool
AosTaskMgr::taskFinishedLockedInMem(
		const u64 &task_docid, 
		const u32 logic_pid,
		const AosRundataPtr &rdata)
{
	//do nothing for now
	return true;
}


bool
AosTaskMgr::taskStartedLockedInMem(
		const u64 &task_docid,
		const u32 logic_pid,
		const AosRundataPtr &rdata)
{
OmnScreen << "==========================task started : " << task_docid << " : " << logic_pid << endl;
	return true;
}


bool
AosTaskMgr::taskFailedLockedInMem(
		const u64 &task_docid,
		const u32 logic_pid,
		const AosRundataPtr &rdata)
{
	//do nothing for now
	return true;
}

bool
AosTaskMgr::getStartTaskInfoLocked(const u32 logic_pid, u64 &task_docid, AosRundataPtr &rdata)
{
	task_docid = 0;
	rdata = 0;
	map<u64, TaskInfo>::iterator itr = mStartTasks.begin();
	while(itr != mStartTasks.end())
	{
		if ((itr->second).mLogicPid == logic_pid)
		{
			task_docid = (itr->second).mTaskDocid;
			rdata = (itr->second).mRundata;
			break;
		}
		itr++;
	}
	return true;
}

u32
AosTaskMgr::getStartTaskPidLocked(const u64 &task_docid)
{
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	return (itr->second).mLogicPid;
}

bool
AosTaskMgr::removeWaitProcLocked(const u32 logic_pid)
{
	deque<u32>::iterator itr = mWaitProcs.begin();
	while(itr != mWaitProcs.end())
	{
		if ((*itr) == logic_pid)
		{
OmnScreen << "task.exe may be killed or core dump, logic_pid: " << logic_pid << endl;
			mWaitProcs.erase(itr);
			mProcIdMap.erase(logic_pid);
			return true;
		}
		itr++;
	}
	return true;
}

bool
AosTaskMgr::removeWaitTaskLocked(const u64 &task_docid)
{
	deque<TaskInfo>::iterator itr = mWaitTasks.begin();
	while(itr != mWaitTasks.end())
	{
		if (task_docid == (*itr).mTaskDocid)
		{
			mWaitTasks.erase(itr);
			return true;
		}
		itr++;
	}
	return true;
}

bool
AosTaskMgr::cleanOutPuts(vector<AosXmlTagPtr> &outputs, const AosRundataPtr &rdata)
{

	OmnScreen << "=============removeStartTask, mOutPuts size: " << outputs.size() << endl;
	AosXmlTagPtr taskdata_tag;
	for (u32 i=0; i < outputs.size(); i++)
	{
		taskdata_tag = outputs[i];
		OmnString str_type = taskdata_tag->getAttrStr(AOSTAG_TYPE, "");
		AosTaskDataType::E type = AosTaskDataType::toEnum(str_type);
		if (type == AosTaskDataType::eOutPutDoc || 
				type == AosTaskDataType::eOutPutIIL)
		{
			u64 file_id = taskdata_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			int physicalid = taskdata_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
			bool svr_death = false;
			OmnScreen << "**********************delete file cleanTaskDocOutPut, file_id: " 
				<< file_id << " , physicalid: " << physicalid << endl;
			AosNetFileCltObj::deleteFileStatic(
				file_id, physicalid, svr_death, rdata.getPtr());
			if (svr_death) AosRecycle::getSelf()->diskRecycle(physicalid, file_id, rdata);
			OmnScreen << "taskFailed; input_error; or output_error;" 
				<< "; clean file; physicalid:" << physicalid << "; file_id:" << file_id 
				<< "; svr_death:" << (svr_death?"true":"false")
				<< endl;
		}
	}
	return true;
}

bool
AosTaskMgr::rollBackSnapShots(
			const u64 &task_docid,
			const AosXmlTagPtr &snapshots_tag,
			const AosRundataPtr &rdata)
{
	if (!snapshots_tag)
	{
		return true;
	}
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);

	bool rslt = false;
	u32 virtual_id = 0;
	u64 snapshot_id = 0;
	AosTaskDataType::E type;
	AosDocType::E doctype = AosDocType::eNormalDoc;
	AosXmlTagPtr snapshot_tag = snapshots_tag->getFirstChild(true);
	while(snapshot_tag)
	{
		AosTaskDataObjPtr snapshot = AosTaskData::serializeFromStatic(snapshot_tag, rdata);
		aos_assert_r(snapshot, false);
		virtual_id = snapshot->getVirtualId();		
		snapshot_id = snapshot->getSnapShotId();
		type = snapshot->getTaskDataType();
		if (type == AosTaskDataType::eDocSnapShot)
		{
			doctype = snapshot->getDocType();
			rslt = doc_client->rollbackSnapshot(rdata, virtual_id, doctype, snapshot_id, task_docid);
			aos_assert_r(rslt, false);
		}
		else if (type == AosTaskDataType::eIILSnapShot)
		{
			rslt = iil_client->rollBackSnapshot(virtual_id, snapshot_id, task_docid, rdata);
			aos_assert_r(rslt, false);
		}
OmnScreen << "========================taskMgr to rollback....: " << snapshot->getStrKey(rdata) << endl;
		snapshot_tag = snapshots_tag->getNextChild();
	}
	return true;
}

bool
AosTaskMgr::removeStartTaskLocked(const u64 &task_docid, const bool isclean, const AosRundataPtr &rdata)
{
	OmnScreen << "=============removeStartTask: " << task_docid << endl;
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if(itr != mStartTasks.end())
	{
		if (isclean)
		{
			bool rslt = cleanOutPuts((itr->second).mOutPuts, rdata);
			aos_assert_r(rslt, false);
			AosXmlTagPtr snapshots_tag = (itr->second).mSnapShots;
			if (snapshots_tag)
			{
				rslt = rollBackSnapShots(task_docid, snapshots_tag, rdata);
				aos_assert_r(rslt, false);
			}
		}
		mStartTasks.erase(itr);
	}
	return true;
}


bool 
AosTaskMgr::getTaskRunnerInfos(
		const u64 &job_docid,
		const u32 num_slots,
		const int crt_jobsvrid,
		vector<AosTaskRunnerInfoPtr> &run_infos,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job_docid != 0 && num_slots > 0, false);
	run_infos.clear();
	AosTaskRunnerInfoPtr run_info;
	u64 task_docid = 0;
	AosTaskStatus::E status;
	mLock->lock();
OmnScreen << "--------------------------------------------------------------------------" << endl;
	map<u64, TaskInfo>::iterator itr = mStartTasks.begin();
	while (itr != mStartTasks.end())
	{
		if ((itr->second).mJobDocid != job_docid)
		{
			itr++;
			continue;
		}
		task_docid = (itr->second).mTaskDocid;
		aos_assert_rl(task_docid, mLock, false);
		if ((itr->second).mCrtJobSvrId != crt_jobsvrid)
		{
			(itr->second).mCrtJobSvrId = crt_jobsvrid;
		}
		status = (itr->second).mStatus; 
		run_info = OmnNew AosTaskRunnerInfo(
				(itr->second).mJobDocid, 
				(itr->second).mLogicId, 
				(itr->second).mTaskType, 
				task_docid, 
				AosGetSelfServerId(), 
				status , 
				(itr->second).mProgress, 
				(itr->second).mStartStamp,
				(itr->second).mErrorType, 
				(itr->second).mErrmsg);
		run_infos.push_back(run_info);
OmnScreen << "job_docid: " << (itr->second).mJobDocid 
		  << " , logic_id: " << (itr->second).mLogicId 
		  << " , task_docid: " << task_docid
		  << " , status: " << AosTaskStatus::toStr(status) << endl;
		if (status == AosTaskStatus::eFinish)
		{
			run_info->setTaskOutPuts((itr->second).mOutPuts);
			run_info->setTaskProcNum((itr->second).mProcNum);
			run_info->setProgress(100);
			run_info->setTaskSnapShots((itr->second).mSnapShots);

			u64 logic_pid = (itr->second).mLogicPid;
			itr++;
			taskFinishedLocked(task_docid, logic_pid, rdata);
		}
		else if (status == AosTaskStatus::eFail)
		{
			u64 logic_pid = (itr->second).mLogicPid;
			itr++;
			taskFailedLocked(task_docid, logic_pid, rdata);
		}
		else
		{
			itr++;
		}
	}
	aos_assert_r(run_infos.size() <= num_slots, false);
	u32 last = num_slots - run_infos.size();
	for (u32 i=0; i<last; i++)
	{
OmnScreen << "*****************idle*****************" << endl;
		run_info = OmnNew AosTaskRunnerInfo(job_docid, AosGetSelfServerId(), true);
		run_infos.push_back(run_info);
	}
OmnScreen << "--------------------------------------------------------------------------" << endl;
	mLock->unlock();
	return true;
}


u32 
AosTaskMgr::getLogicPid(u64 taskDocId)
{
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(taskDocId);

	if (itr == mStartTasks.end())
		return 0;

	TaskInfo info = itr->second;
	return info.mLogicPid;
}

bool
AosTaskMgr::isService(AosRundataPtr rdata, u64 docid)
{
	//get taskdoc firstly
	AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata); 
	aos_assert_r(doc, false);

	OmnString  serviceName = doc->getAttrStr(AOSTAG_SERVICE_NAME, "");
	OmnTagFuncInfo << "service name is: " <<  serviceName << endl;
	if (serviceName == "")
		return false;

	return true;
}


AosBuffPtr
AosTaskMgr::getRunTimeValue(
			const u64 &job_docid,
			const u64 &task_docid,
			const OmnString &key,
			const AosRundataPtr &rdata)
{
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return 0;
	}
	int job_svrid= (itr->second).mCrtJobSvrId;
	mLock->unlock();
	AosBuffPtr resp_buff;
	AosTransPtr trans = OmnNew AosGetRunTimeValueByJobTrans(key, job_docid, job_svrid);
	bool rslt = addReq(rdata, trans, resp_buff);
	aos_assert_r(rslt && resp_buff, 0);
	return resp_buff;
}


AosBuffPtr
AosTaskMgr::setRunTimeValue(
			const u64 &job_docid,
			const u64 &task_docid,
			const OmnString &key,
			const AosValueRslt &value,
			const AosRundataPtr &rdata)
{
	mLock->lock();
	map<u64, TaskInfo>::iterator itr = mStartTasks.find(task_docid);
	if (itr == mStartTasks.end())
	{
		mLock->unlock();
		return 0;
	}
	int job_svrid= (itr->second).mCrtJobSvrId;
	mLock->unlock();
	AosBuffPtr resp_buff;
	AosTransPtr trans = OmnNew AosSetRunTimeValueByJobTrans(key, value, job_docid, job_svrid);
	bool rslt = addReq(rdata, trans, resp_buff);
	aos_assert_r(rslt && resp_buff, 0);
	return resp_buff;
}

bool
AosTaskMgr::clearIILEntryMap(const AosRundataPtr &rdata)
{
	mLock->lock();
	deque<u32>::iterator itr = mWaitProcs.begin();
	deque<u32>::iterator tmp_itr;
	while(itr != mWaitProcs.end())
	{
		AosBuffPtr resp_buff;
		bool timeout = false;
		AosTransPtr trans = OmnNew AosClearTaskIILEntryMapTrans(AosGetSelfServerId(), *itr);
		AosSendTrans(rdata, trans, timeout, resp_buff);  
		if (!resp_buff)
		{
			OmnScreen << "jozhi task.exe process may be not started, " 
				<< "remove it from queue proc logicid : " << *itr << endl;
			tmp_itr = itr;
			itr++;
			mWaitProcs.erase(tmp_itr);
			mProcIdMap.erase(*tmp_itr);
			continue;
		}
		aos_assert_r(resp_buff && resp_buff->getU8(0), false);
		itr++;
	}
	mLock->unlock();
	return true;
}

