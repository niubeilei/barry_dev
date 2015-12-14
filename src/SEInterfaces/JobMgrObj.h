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
// 05/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JobMgrObj_h
#define Aos_SEInterfaces_JobMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosJobMgrObj : virtual public OmnRCObject
{
	static AosJobMgrObjPtr	smJobMgr;

public:
	enum ProcStatus
	{
		eStarted = 1,
		eUnusable
	};

	enum OprId
	{
		eInvalid,
		eNormal,
		eReStart,
		eSwitchJobMaster,
	};

public:
	static void setJobMgr(const AosJobMgrObjPtr &d) {smJobMgr = d;}
	static AosJobMgrObjPtr getJobMgr() {return smJobMgr;}

	virtual bool reStartJob(
					const OmnString &args,
					const OmnString &job_objid,
					const AosRundataPtr &rdata) = 0;

	virtual bool startJob(
					const OmnString &args,
					const OmnString &job_objid,
					const AosXmlTagPtr &doc,
					const AosRundataPtr &rdata) = 0;

	//virtual bool startJob(
	//				const OmnString &args,
	//				const OmnString &service_name,  
	//				const OmnString &job_objid,
	//				const AosXmlTagPtr &doc,
	//				const AosRundataPtr &rdata) = 0;

	virtual bool stopJob(
					const OmnString &args,
					const OmnString &job_objid,
					const AosRundataPtr &rdata) = 0;

	//Jozhi delete
	//virtual bool failJob(
	//				const OmnString &args,
	//				const OmnString &job_objid,
	//				const AosRundataPtr &rdata) = 0;

	//virtual bool pauseJob(
	//				const OmnString &args,
	//				const OmnString &job_objid,
	//				const AosRundataPtr &rdata) = 0;

	//virtual bool finishJob(
	//				const OmnString &args,
	//				const OmnString &job_objid,
	//				const AosRundataPtr &rdata) = 0;

	//virtual bool jobStarted(
	//				const AosJobObjPtr &job,
	//				const AosRundataPtr &rdata) = 0;

	virtual bool jobStoped(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata) = 0;

	//virtual bool jobPaused(
	//				const AosJobObjPtr &job,
	//				const AosRundataPtr &rdata) = 0;

	virtual bool jobFailed(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata) = 0;

	virtual bool jobFinished(
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata) = 0;

	virtual AosJobObjPtr getStartJob(
					const u64 &job_docid,
					const AosRundataPtr &rdata) = 0;

	virtual AosJobObjPtr getStartJobLocked(
					const u64 &job_docid,
					const AosRundataPtr &rdata) = 0;

	virtual bool heartbeatCallBack(
					const bool is_svr_death,
					const int svr_id) = 0;

	virtual bool checkStartJobs() = 0;
	virtual bool sendHeartbeat() = 0;

	virtual bool svrDeath(const int death_svr_id) = 0;

	virtual bool getTaskRunnerInfoBySvrId(
					const AosRundataPtr &rdata, 
					const int svr_id) = 0;

	virtual bool scheduleNextTask(
					const AosTaskRunnerInfoPtr &runner_info) = 0;
	
	virtual ProcStatus getProcStatus(const u32 svr_id) = 0;

	virtual int 	getNumUnusableProc() = 0;

	virtual void isRunJob(const bool flag) = 0;
	virtual bool tryStartJob() = 0;
	
	virtual bool checkDiskFreeSizeLocal(
					const AosRundataPtr &rdata) = 0;


	//Jozhi delete
	//virtual AosXmlTagPtr composeJobXml(
	//		const AosRundataPtr &rdata,
	//		const OmnString &objid) = 0;
};

#endif
