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
// 08/09/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_TaskMgr_h
#define AOS_TaskMgr_TaskMgr_h

#include "ErrorMgr/ErrorCode.h"
#include "SEInterfaces/CommandRunnerObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Job/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransProc.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "API/AosApi.h"

#include <deque>
#include <hash_map>
using namespace std;


OmnDefineSingletonClass(AosTaskMgrSingleton,
						AosTaskMgr,
						AosTaskMgrSelf,
						OmnSingletonObjId::eTaskMgr,
						"TaskMgr");


class AosTaskMgr : public OmnThreadedObj,
				   public AosTaskMgrObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxWaitTask = 100,
		eMaxStartTask = 50 
	};

private:
	struct TaskInfo
	{
		u64						mJobDocid;
		OmnString				mLogicId;
		OmnString				mTaskType;
		u32 					mLogicPid;
		u64 					mTaskDocid;
		u64 					mStartStamp;
		int						mProgress;
		AosTaskStatus::E    	mStatus;
		AosTaskErrorType::E 	mErrorType;
		OmnString				mErrmsg;
		vector<AosXmlTagPtr>	mOutPuts;	
		AosXmlTagPtr			mProcNum;
		AosRundataPtr 			mRundata;
		int						mCrtJobSvrId;
		AosXmlTagPtr			mSnapShots;
	};

private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr                mStartTaskThread;
	deque<TaskInfo>				mWaitTasks;
	map<u64, TaskInfo>			mStartTasks;
	deque<u32>					mWaitProcs;
	map<u32, int>				mProcIdMap;
	int							mNumTask;
	int							mNumProc;

	
public:
	AosTaskMgr();
	~AosTaskMgr();

//====================test
bool						mIsTest;
    // Singleton class interface
    static AosTaskMgr *    	getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);

	//logicpid method
	virtual u32 getLogicPid(u64 taskDocId);

	// ThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    threadFuncInMem(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

	static bool isService(AosRundataPtr rdata, u64 docid);

	bool startTask(const u64 &task_docid, const u32 logic_pid, const AosRundataPtr &rdata);
	virtual bool stopTask(const u64 &task_docid, const AosRundataPtr &rdata);
	virtual bool clearIILEntryMap(const AosRundataPtr &rdata);


	
	virtual bool addTask(
				const int crt_jobsvrid,
				const u64 &task_docid, 
				const AosRundataPtr &rdata);

	virtual bool addTaskLocal(
				const u64 &job_docid,
				const OmnString &logic_id,
				const OmnString &task_type,
				const int crt_jobsvrid,
				const u64 &task_docid,
				const u64 &start_stamp,
				const AosRundataPtr &rdata);

	virtual bool startTaskProcCb(const u32 logic_pid, const int proc_pid, const int svr_id);
	virtual bool stopTaskProcCb(const u32 logic_pid, const int svr_id);

	virtual bool updateTaskStatus(
				const AosTaskStatus::E &status,
				const AosTaskErrorType::E &error_type,
				const OmnString &error_msg,
				const u32 logic_pid,
				const u64 &task_docid, 
				const AosRundataPtr &rdata);

	virtual bool getTaskRunnerInfos(
				const u64 &job_docid,
				const u32 num_slots,
				const int crt_jobsvrid,
				vector<AosTaskRunnerInfoPtr> &run_info,
				const AosRundataPtr &rdata);

	virtual	bool updateTaskProgress(
				const u64 &task_docid,
				const int percent,
				const AosRundataPtr &rdata);

	virtual bool updateTaskOutPut(
				const u64 &task_docid,
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	virtual bool updateTaskSnapShots(
				const u64 &task_docid,
				const AosXmlTagPtr &snapshots_tag,
				const AosRundataPtr &rdata);

	virtual bool updateTaskProcNum(
				const u64 &task_docid,
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	virtual bool getDataColOutput(
				const OmnString &datacol_id,
				const u64 &job_id,
				const u64 &task_docid,
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	virtual bool createDataCol(
				const OmnString &datacol_id,
				const u64 &job_id,
				const u64 &task_docid,
				const OmnString &config,
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);


	virtual AosBuffPtr getRunTimeValue(
				const u64 &job_docid,
				const u64 &task_docid,
				const OmnString &key,
				const AosRundataPtr &rdata);

	virtual AosBuffPtr setRunTimeValue(
				const u64 &job_docid,
				const u64 &task_docid,
				const OmnString &key,
				const AosValueRslt &value,
				const AosRundataPtr &rdata);

private:
	bool 	addReq(
				const AosRundataPtr &rdata,
				const AosTransPtr &trans)
	{
		bool rslt = AosSendTrans(rdata, trans);
		aos_assert_rr(rslt, rdata, false);
		rdata->setOk();
		return true;
	}

	bool 	addReq(
				const AosRundataPtr &rdata,
				const AosTransPtr &trans,
				AosBuffPtr &resp)
	{
		bool timeout = false;
		bool rslt = AosSendTrans(rdata, trans, timeout, resp);
		aos_assert_r(rslt, false);
		aos_assert_r(!timeout, false);
		aos_assert_r(resp && resp->dataLen() >= 0, false);
		return true;
	}
	bool taskFinishedLocked(const u64 &task_docid, const u32 task_pid, const AosRundataPtr &rdata);
	bool taskFailedLocked(const u64 &task_docid, const u32 task_pid, const AosRundataPtr &rdata);
	bool taskStartedLocked(const u64 &task_docid, const u32 task_pid, const AosRundataPtr &rdata);

	bool taskFinishedLockedInMem(const u64 &task_docid, const u32 task_pid, const AosRundataPtr &rdata);
	bool taskFailedLockedInMem(const u64 &task_docid, const u32 task_pid, const AosRundataPtr &rdata);
	bool taskStartedLockedInMem(const u64 &task_docid, const u32 task_pid, const AosRundataPtr &rdata);

	bool isFindWaitProcLocked(const u32 logic_pid);
	bool isFindWaitTaskLocked(const u64 &task_docid);
	bool isFindStartTaskLocked(const u64 &task_docid);
	u32  getStartTaskPidLocked(const u64 &task_docid);
	bool getStartTaskInfoLocked(const u32 logic_pid, u64 &task_docid, AosRundataPtr &rdata);
	bool removeWaitTaskLocked(const u64 &task_docid);
	bool removeWaitProcLocked(const u32 logic_pid);
	bool removeStartTaskLocked(const u64 &task_docid, const bool isclean, const AosRundataPtr &rdata);
	bool killProcPriv(const u32 logic_pid);
	bool realKillProcPriv(const u32 logic_pid);
	bool updateTaskStatusLocked(
				const AosTaskStatus::E &status,
				const AosTaskErrorType::E &error_type,
				const OmnString &error_msg,
				const u32 logic_pid,
				const u64 &task_docid, 
				const AosRundataPtr &rdata);

	bool cleanOutPuts(
			vector<AosXmlTagPtr> &outputs,
			const AosRundataPtr &rdata);

	bool rollBackSnapShots(
			const u64 &task_docid,
			const AosXmlTagPtr &snapshots_tag,
			const AosRundataPtr &rdata);
};
#endif
