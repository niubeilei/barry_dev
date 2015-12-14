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
#ifndef AOS_Job_JobMgr_h
#define AOS_Job_JobMgr_h

#include "Job/Ptrs.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "NetworkMgrUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/CommandRunnerObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransUtil/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransProc.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util1/TimerObj.h"
#include <queue>
using namespace std;


OmnDefineSingletonClass(AosJobMgrSingleton,
						AosJobMgr,
						AosJobMgrSelf,
						OmnSingletonObjId::eJobMgr,
						"JobMgr");


class AosJobMgr : public OmnThreadedObj,
				  public AosCommandRunnerObj,
				  public AosJobMgrObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxWaitJobs = 100,
		eMaxStartJobs = 50,
		eMaxFailTasks = 100,
		eProcUnusablePercent = 70
	};

private:
	struct JobInfo
	{
		OprId 		mOpr;
		u64 		mJobDocid;
		AosXmlTagPtr mJobDoc;
		AosXmlTagPtr mJobSdoc;
		AosRundataPtr mRundata;
		OmnString mArgs;
	};

	enum
	{
		eDftMaxJobs = 10000,
		eDftMaxRunningJobs = 100
	};

	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	OmnThreadPtr			mThread;
	OmnThreadPtr			mTimerThread;
	deque<JobInfo>			mWaitJobInfos;
	//deque<AosJobObjPtr>	mWaitJobs;
	map<u64, AosJobObjPtr> 	mStartJobs;
	AosRundataPtr			mRundata;
	bool					mThreadStatus1;
	bool					mThreadStatus2;
	bool					mThreadStatus3;
	map<u32, ProcStatus>  	mProcStatus;	
	bool					mIsRunJob;
	
	u64						mSafeDiskFreeSize;
	set<OmnString>			mDiskNameSet;
	
	AosJobMgr();
	~AosJobMgr();

public:
    // Singleton class interface
    static AosJobMgr * getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);

	// AosThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	//virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual bool    checkThread111(OmnString &err, const int thrdLogicId);

	// AosCommandRunnerObj interface
	virtual bool runCommand(
					const OmnString &args, 
					const AosXmlTagPtr &root, 
					const AosRundataPtr &rdata);

	//job command form browser or ...
	virtual bool reStartJob(
					const OmnString &args,
					const OmnString &job_objid,
					const AosRundataPtr &rdata);

	virtual bool startJob(
					const OmnString &args,
					const OmnString &job_objid,
					const AosXmlTagPtr &doc,
					const AosRundataPtr &rdata);

	virtual bool stopJob(
					const OmnString &args,
					const OmnString &job_objid,
					const AosRundataPtr &rdata);

	virtual bool jobStoped(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata);

	virtual bool jobFailed(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata);

	virtual bool jobFinished(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata);

	virtual AosJobObjPtr getStartJob(
					const u64 &job_docid,
					const AosRundataPtr &rdata);

	virtual AosJobObjPtr getStartJobLocked(
					const u64 &job_docid,
					const AosRundataPtr &rdata);
	
	virtual  bool 	checkStartJobs();
	virtual  bool 	sendHeartbeat();

	virtual bool   	heartbeatCallBack(
					const bool is_svr_death,
					const int svr_id);

	virtual bool	svrDeath(const int death_svr_id);

	virtual bool 	scheduleNextTask( const AosTaskRunnerInfoPtr &runner_info);

	virtual int		getNumUnusableProc();

	virtual AosJobMgrObj::ProcStatus getProcStatus(const u32 svr_id);

	virtual bool	getTaskRunnerInfoBySvrId(const AosRundataPtr &rdata, const int svr_id);
	
	virtual bool checkDiskFreeSize(const AosRundataPtr &rdata);
	virtual bool checkDiskFreeSizeLocal(const AosRundataPtr &rdata);

	virtual bool tryStartJob();

	virtual void isRunJob(const bool flag){mIsRunJob = flag;};
private:
	bool removeStartJob(
			const AosJobObjPtr &job,
			const AosRundataPtr &rdata);
	bool removeWaitJob(
			const u64 &job_docid);

	bool getWaitJob(
			const u64 &job_docid,
			const AosRundataPtr &rdata);

	bool mainThreadFunc(
			OmnThrdStatus::E &state,
			const OmnThreadPtr &thread);

	bool timerThreadFunc(
			OmnThrdStatus::E &state,
			const OmnThreadPtr &thread);

	bool addReq(
			const AosRundataPtr &rdata,
			const AosTransPtr &trans);

	bool addReq(
			const AosRundataPtr &rdata,
			const AosTransPtr &trans,
			AosBuffPtr &resp);

	bool startJob(
			const bool isReStart,
			const AosXmlTagPtr &job_doc,
			const AosXmlTagPtr &job_sdoc,
			const AosRundataPtr &rdata);

	bool createJobObj(
			const OmnString &sdoc_objid,
			const AosRundataPtr rdata);

	void setProcStatus(const u32 svr_id, const ProcStatus status);

	void parseTimerSdoc(const AosRundataPtr &rdata);

	bool hasSingleStartJob();

	AosJobObjPtr getJobByObjid(
			const OmnString &objid,
			const AosRundataPtr &rdata);

	AosJobObjPtr getJobByDocid(
			const u64 job_docid,
			const AosRundataPtr &rdata);

	AosJobObjPtr getJobByDocidPriv(
			const u64 job_docid,
			const AosRundataPtr &rdata);

	AosJobObjPtr getJobByObjidPriv(
			const OmnString &objid,
			const AosRundataPtr &rdata);

	AosJobObjPtr createJob(
			const OprId	id,
			const AosXmlTagPtr &job_doc,
			const AosXmlTagPtr &job_sdoc,
			const AosRundataPtr &rdata);

	bool startJobBySwitchMaster(
			const OmnString &args,
			const OmnString &job_objid,
			const AosRundataPtr &rdata);
};
#endif

