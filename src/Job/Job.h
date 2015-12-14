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
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Job_Job_h
#define AOS_Job_Job_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataCollector/DataCollector.h"
#include "DataCollector/DataCollectorMgr.h"
#include "Job/Ptrs.h"
#include "Job/JobId.h"
#include "Job/JobRunTime.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/JobStatus.h"
#include "SEInterfaces/LogicTaskObj.h"
#include "TaskUtil/DataColCtlr.h"
#include "TaskScheduler/TaskScheduler.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/Task.h"

#include <vector>
#include <set>
#include <map>
#include <list>

#include <ostream>
#include <sstream>


//barry 2015/11/12
//JIMODB-401
#define  AosTaskLog(x) \
	TaskLog &log = TaskLog::smTaskLog;\
	std::ostringstream os; \
	log.getLogStr((u64)x, os); \
	os\

#define  AosJobLog(x) \
	JobLog &log  = JobLog::smJobLog;\
	OmnString file;file<<__FILE__;OmnString line;line<<__LINE__;\
	std::ostringstream os; \
	log.getLogStr((u64)x, os, file, line);\
	os\

#define endlog \
		"";\
		log.log(os);\


class JobLog
{
	public:
		static JobLog  smJobLog;

	public:
		void getLogStr(
				const u64 job_id, 
				std::ostringstream &os,
				const OmnString &file,
				const OmnString &line);

		static void log(const std::ostringstream &data);
};


using namespace std;

class AosJob : public AosJobObj
{
public:
	enum
	{
		eMaxTaskFailNum = 20
	};

private:
	struct SnapShotInfo
	{
		AosXmlTagPtr 		mSnapTag;
		AosTaskDataObjPtr	mSnapShot;
	};
	OmnMutexPtr										mLock;
	AosJobId::E										mType;
	AosJobStatus::E									mStatus;
	bool											mIsPublic;
	bool											mIsDeleteFile;
	bool 											mSingleRun;
	int												mJobServerId;
	int												mJobProgress;
	u64												mJobDocid;
	OmnString										mJobName;
	OmnString										mStartTime;
	OmnString										mFinishTime;
	bool											mThrdShellError;
	OmnString										mErrorMsg;
	OmnString										mJobContainerObjid;
	OmnString										mLogContainerObjid;
	AosXmlTagPtr									mJobDoc;
	AosXmlTagPtr									mJobSdoc;
	AosTaskSchedulerPtr								mScheduler;
	AosRundataPtr									mRundata;
	set<OmnString>									mNeedLogAttrs;
	map<OmnString, OmnString>						mJobENV;
	map<OmnString, u64>								mJobProcNum;
	map<OmnString, AosLogicTaskObjPtr>				mLogicTasks;
	int												mVersion;
	set<OmnString>									mLogicTaskSet;
	vector<OmnString>								mLogicRunQueue;
	map<u32, SnapShotInfo>							mIILSnapShots;
	map<u32, SnapShotInfo>							mDocSnapShots;
	AosDataCollectorMgrPtr							mDataColMgr;
	AosJobRunTimePtr								mJobRunTime;
	set<u64>										mDeleteTaskDocs;
	map<OmnString, AosXmlTagPtr>					mOutPutResources;
	map<OmnString, AosXmlTagPtr>					mReduceResources;

	class SendClearIILEntryMapThrd: public OmnThrdShellProc
	{
		OmnDefineRCObject;
		private:
		int             mPhysicalId;
		AosRundataPtr   mRundata;
		public:
		SendClearIILEntryMapThrd(
				const int physicalId,
				const AosRundataPtr &rdata)
			:
				OmnThrdShellProc("SendClearIILEntryMapThrd"),
				mPhysicalId(physicalId),
				mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};

	class SendClearCubeIILEntryMapThrd: public OmnThrdShellProc
	{
		OmnDefineRCObject;
		private:
		u32             mCubeId;
		AosRundataPtr   mRundata;
		public:
		SendClearCubeIILEntryMapThrd(
				const u32 cube_id,
				const AosRundataPtr &rdata)
			:
				OmnThrdShellProc("SendClearCubeIILEntryMapThrd"),
				mCubeId(cube_id),
				mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};

	class createQueryThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;
	private:
		AosJob * 		mJob;
		int 			mStartIdx;
		bool			mCleanTaskData;
		OmnString		mLogicId;
		AosJobMgrObj::OprId mOprid;
		AosRundataPtr	mRundata;
	public:
		createQueryThrd(
				AosJob* job,
				const int startIdx,
				const OmnString &logic_id,
				const AosJobMgrObj::OprId oprid,
				const AosRundataPtr &rdata)
			:
		OmnThrdShellProc("createQueryThrd"),             
		mJob(job),                                       
		mStartIdx(startIdx),                             
		mCleanTaskData(false),                           
		mLogicId(logic_id),
		mOprid(oprid),                                   
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
			aos_assert(mLogicId != "");
		}
		createQueryThrd(
				AosJob * job,
				const int startIdx,
				const AosJobMgrObj::OprId oprid,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("createQueryThrd"),
		mJob(job),
		mStartIdx(startIdx),
		mCleanTaskData(false),
		mOprid(oprid),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		createQueryThrd(
				AosJob * job,
				const int startIdx,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("createQueryThrd"),
		mJob(job),
		mStartIdx(startIdx),
		mCleanTaskData(true),
		mOprid(AosJobMgrObj::eInvalid),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};

public:
	AosJob(const AosJobId::E type, const OmnString &name, const bool flag);
	~AosJob();

	virtual u32							getNumSlots() const;
	virtual int 						getVersion();
	virtual int 						getJobPhysicalId() const;
	virtual void 						clear();
	virtual void 						setRundata(
											const AosRundataPtr &rdata);
	virtual bool 						isJobPublic() const;
	virtual bool 						getSingleRunningStatus() const;
	virtual bool 						isStoped() const;
	virtual bool 						isStarted() const;
	virtual bool 						isFinished() const;
	virtual bool 						isWaited() const;
	virtual bool 						isFailed() const;
	virtual bool 						getIsDeleteFile() const;
	virtual bool 						checkStartJob();
	virtual	bool 						clearSnapShots();
	virtual bool 						startJob(
											const AosJobMgrObj::OprId opr_id,
											const AosRundataPtr &rdata);
	virtual bool 						stopJob(
											const AosRundataPtr &rdata);
	virtual bool 						updateJobProgress(
											const AosRundataPtr &rdata);
	virtual bool 						checkJobFinishing(
											const AosRundataPtr &rdata);
	virtual bool 						removeWaitListByTaskId(
											const u64 &task_docid);
	virtual bool						 updateJobFailTaskNum(
											const AosRundataPtr &rdata);
	virtual bool 						svrDeath(
											const int svr_id);
	virtual	bool 						commit(
											const AosRundataPtr &rdata);
	virtual bool 						setStatus(
											const AosJobStatus::E status, 
											const AosRundataPtr &rdata);
	virtual bool						addRestartFinishResources(
											const AosXmlTagPtr &task_doc,
											const AosRundataPtr &rdata);
	virtual bool						addRestartReduceResources(
											const AosXmlTagPtr &task_doc,
											const AosRundataPtr &rdata);
	virtual bool 						updateSnapShots(
											const AosXmlTagPtr &snapshots,
											const AosRundataPtr &rdata);
	virtual bool 						addTaskInfo(
											const AosTaskInfoPtr &info,
											const AosRundataPtr &rdata);
	virtual bool 						logJob(
											const OmnString &entry_key,
											const AosRundataPtr &rdata);
	virtual bool 						jobFailed(
											const AosRundataPtr &rdata,
											const bool run_cond);
	virtual bool 						scheduleNextTask(
											const AosTaskRunnerInfoPtr &run_info,
											const AosRundataPtr &rdata);
	virtual bool 						returnSchedulerActiveTask(
											const int task_svr_id, 
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	virtual bool 						recoverSchedulerActiveTask(
											const int task_svr_id, 
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	virtual bool 						initJob(
											const AosJobMgrObj::OprId id,
											const AosXmlTagPtr &jobdoc,
											const AosXmlTagPtr &sdoc, 
											const AosRundataPtr &rdata);
	virtual bool 						addErrorMsg(
											const AosXmlTagPtr &job_doc,
											const OmnString &error_msg,
											const AosRundataPtr &rdata);
	virtual bool 						setRunTimeValue(
											const OmnString &key,
											const AosValueRslt &value,
											const AosRundataPtr &rdata);
	virtual bool 						taskFailed(
											const AosXmlTagPtr &task_doc,
											const u64 &task_stime,
											const AosRundataPtr &rdata);
	virtual bool 						isValid(
											const u64 &task_docid,
											const u64 &stime,
											const AosRundataPtr &rdata);
	virtual u64 						getJobDocid() const;
	virtual OmnString 					getTaskContainer() const;
	virtual OmnString 					getLogContainer() const;
	virtual OmnString 					getJobENV(const OmnString &key);
	virtual OmnString 					getJobObjid() const;
	virtual AosXmlTagPtr 				getJobDoc() const;
	virtual AosXmlTagPtr 				getSnapShots(
											const AosRundataPtr &rdata);
	virtual AosRundataPtr 				getRundata() const;
	virtual map<OmnString, OmnString>& 	getJobENV();

	virtual AosLogicTaskObjPtr 			getLogicTask(
											const OmnString &logic_id,
											const AosRundataPtr &rdata);
	virtual AosLogicTaskObjPtr 			getLogicTaskLocked(
											const OmnString &logic_id,
											const AosRundataPtr &rdata);
	virtual AosDataCollectorMgrPtr 		getDataColMgr();
	virtual AosValueRslt 				getRunTimeValue(
											const OmnString &key,
											const AosRundataPtr &rdata);
	virtual AosJobObjPtr 				create(
											const AosJobMgrObj::OprId id,
											const AosXmlTagPtr &jobdoc,
											const AosXmlTagPtr &sdoc,
											const AosRundataPtr &rdata) const;

	static bool							staticInit();
	static AosJobObjPtr 				staticCreateJob(
											const AosJobMgrObj::OprId id,
											const AosXmlTagPtr &jobdoc, 
											const AosXmlTagPtr &sdoc, 
											const AosRundataPtr &rdata);
private:
	bool								finishDataCollector(
											const AosRundataPtr &rdata);
	bool 								clearIILEntryMap(
											const AosRundataPtr &rdata);
	bool 								checkLogicTaskFinished(
											const AosRundataPtr &rdata);
	bool 								checkDataColFinished(
											const AosRundataPtr &rdata);
	bool 								initSnapShots(
											const AosRundataPtr &rdata);
	bool 								reset(
											const AosRundataPtr &rdata);
	bool 								createDataColMgr(
											const AosRundataPtr &rdata);
	bool								scheduleTasks(
											const AosRundataPtr &rdata);
	bool								jobFailedLocked(
											const AosRundataPtr &rdata);
	bool								jobFinishedLocked(
											const AosRundataPtr &rdata);
	bool		 						integrateResources(
											const AosRundataPtr &rdata);
	bool								createJobENV(
											const AosXmlTagPtr &doc,
											const AosRundataPtr &rdata);
	bool								jobStopAllTaskLocked(
											const AosRundataPtr &rdata);
	bool 								cleanAllTaskDataOutPut(
											const AosRundataPtr &rdata);
	bool 								addTaskInfo(
											const AosRundataPtr &rdata,
											const vector<AosTaskInfoPtr> &info);
	bool								createTasksVer1(
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	bool								createTasksByDoc(
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	bool								createTasksByConf(
											const AosXmlTagPtr &sdoc,
											const AosRundataPtr &rdata);
	bool								createSplitter(
											const AosXmlTagPtr &sdoc,
											const AosRundataPtr &rdata);
	bool								createScheduler(
											const AosXmlTagPtr &sdoc,
											const AosRundataPtr &rdata);
	bool								addTaskToWaitList(
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	bool								recycle(
											const AosXmlTagPtr &contents,
											const AosRundataPtr &rdata);
	bool 								startNextLogicTask(
											const AosRundataPtr &rdata,
											const AosJobMgrObj::OprId oprid);
	bool 								triggerAllLogicTasks(
											const AosRundataPtr &rdata, 
											const AosJobMgrObj::OprId oprid);
	bool 								createLogicTasks(
											const AosRundataPtr &rdata,
											const AosXmlTagPtr &sdoc);
	bool 								removeStartTask(
											const AosTaskInfoPtr &info,
											const AosRundataPtr &rdata);
	bool								clearTaskInfo(
											const AosTaskInfoPtr &info,
											const AosRundataPtr &rdata);
	bool 								reStartParentTaskLocked(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	bool								recoverAllTask(
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	bool 								cleanChildTask(
											const set<u64> &child_task_docids,
											const AosRundataPtr &rdata);
	bool 								cleanChildTaskInfoLocked(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	bool 								cleanParentTask(
											const set<u64> &parent_task_docids,
											const AosRundataPtr &rdata);
	bool 								cleanTaskDocOutPut(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	bool								registerJob(
											const AosJob *job,
											const AosJobId::E type,
											const OmnString &name);
	bool								createTasks(
											const AosXmlTagPtr &sdoc,
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	bool								createTasksByDoc(
											const AosXmlTagPtr &contents,
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	bool 								datacollectorFinished(
											const AosRundataPtr &rdata, 
											const OmnString &datacol_id,
											vector<OmnString> &logic_taskid);
	bool 								setSnapShotCommitStatus(
											const AosXmlTagPtr &task_doc,
											const AosTaskDataObjPtr &snapshot,
											const AosRundataPtr &rdata);
	bool 								createMapTask(
											const AosJobMgrObj::OprId oprid,
											const AosLogicTaskObjPtr &logic_task,
											const AosRundataPtr &rdata);
	bool 								createReduceTask(
											const AosJobMgrObj::OprId oprid,
											const AosLogicTaskObjPtr &logic_task,
											const AosRundataPtr &rdata);
	bool 								mergeSnapShotInfo(
											const AosTaskDataObjPtr &target_snap,
											const AosTaskDataObjPtr &merge_snap,
											const AosXmlTagPtr &snap_tag);
	bool 								createJobRunTime(
											const AosXmlTagPtr &job_doc,
											const AosXmlTagPtr &job_sdoc,
											const AosRundataPtr &rdata);
	bool 								coreDumpErrorLocked(
											const AosTaskInfoPtr &info,
											const int server_id,
											const AosRundataPtr &rdata);
	bool 								codeErrorLocked(
											const AosTaskInfoPtr &info,
											const int serverid,
											const AosRundataPtr &rdata);
	bool 								inputErrorLocked(
											const AosTaskInfoPtr &info,
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	bool 								findAllParentTask(
											set<u64> &task_docids, 
											const u64 &task_docid, 
											const AosRundataPtr &rdata);
	bool 								findAllChildTask(
											set<u64> &task_docids, 
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	bool 								cleanDataCol(
											const int level,
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	bool 								killAllStartTask(
											const set<u64> &p_docids,
											const set<u64> &c_docids,
											const AosRundataPtr &rdata);
	bool								tryScheduleAllMapTask(
											const AosJobMgrObj::OprId oprid,
											const OmnString &logic_id,
											const AosRundataPtr &rdata);
	bool								tryScheduleAllReduceTask(
											const AosJobMgrObj::OprId oprid,
											const OmnString &logic_id,
											const AosRundataPtr &rdata);
	bool								tryScheduleTaskPriv(
											const AosJobMgrObj::OprId oprid,
											const AosLogicTaskObjPtr &logic_task,
											const AosRundataPtr &rdata);
	bool 								findRollBackTask(
											set<u64> &pdocids,
											set<u64> &cdocids,
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	vector<u64>* 						findDownStream(
											OmnString dataId); 

	AosTaskInfoPtr						getTaskInfoLocked(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	AosXmlTagPtr 						queryTaskDocs(
											const int startIdx, 
											const AosRundataPtr &rdata);
	AosXmlTagPtr 						queryTaskDocs(
											const int startIdx,
											const OmnString &logic_id,
											const AosRundataPtr &rdata);
	AosXmlTagPtr 						saveSnapShot(
											const AosXmlTagPtr &snapshot_tag,
											const AosRundataPtr &rdata);
	// by Andy zhang 
	void 								showJobInfo(const OmnString &action);

	void								showLogicTaskTotalStart(
										const OmnString &logic_id);
};

#endif

