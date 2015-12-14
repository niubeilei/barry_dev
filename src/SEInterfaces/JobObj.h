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
// 07/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JobObj_h
#define Aos_SEInterfaces_JobObj_h

#include "DataCollector/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JobStatus.h"
#include "SEInterfaces/JobMgrObj.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskStatus.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"

#include <vector>

struct AosJobInfo
{
	u64 							mJobDocid;
	bool 							mJobPublic;
	bool							mIsDeleteFile;
	OmnString						mLogContainer;
	OmnString						mTaskContainer;
	map<OmnString, OmnString>		mJobENV;
};


class AosJobObj : virtual public OmnRCObject
{
	OmnDefineRCObject;
public:
	virtual u32							getNumSlots() const = 0;
	virtual int 						getJobPhysicalId() const = 0;
	virtual int 						getVersion() = 0;
	virtual void 						clear() = 0;
	virtual void 						setRundata(const AosRundataPtr &rdata) = 0; 
	virtual bool 						isJobPublic() const = 0; 
	virtual bool 						getSingleRunningStatus() const = 0;
	virtual bool 						getIsDeleteFile() const = 0;
	virtual bool 						isStoped() const = 0;
	virtual bool 						isStarted() const = 0;
	virtual bool 						isFinished() const =0;
	virtual bool 						isWaited() const =0;
	virtual bool 						isFailed() const =0;
	virtual bool 						checkStartJob() = 0;
	virtual bool 						clearSnapShots() = 0;
	virtual bool 						startJob(
											const AosJobMgrObj::OprId opr_id,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						stopJob(
											const AosRundataPtr &rdata) = 0;
	virtual bool 						updateJobProgress(
											const AosRundataPtr &rdata) = 0;
	virtual bool 						checkJobFinishing(
											const AosRundataPtr &rdata) = 0;
	virtual bool 						removeWaitListByTaskId(
											const u64 &task_docid) = 0;
	virtual bool 						updateJobFailTaskNum(
											const AosRundataPtr &rdata) = 0;
	virtual bool 						svrDeath(
											const int svr_id) = 0;
	virtual bool 						commit(
											const AosRundataPtr &rdata) = 0;
	virtual bool 						jobFailed(
											const AosRundataPtr &rdata, 
											const bool run_cond) = 0;
	virtual bool 						setStatus(
											const AosJobStatus::E status, 
											const AosRundataPtr &rdata) = 0;
	virtual bool 						addTaskInfo(
											const AosTaskInfoPtr &info,
											const AosRundataPtr &rdata) = 0;
	virtual bool						addRestartFinishResources(
											const AosXmlTagPtr &task_doc,
											const AosRundataPtr &rdata) = 0;
	virtual bool						addRestartReduceResources(
											const AosXmlTagPtr &task_doc,
											const AosRundataPtr &rdata) = 0;

	virtual bool 						logJob(
											const OmnString &entry_key,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						updateSnapShots(
											const AosXmlTagPtr &snapshots,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						scheduleNextTask(
											const AosTaskRunnerInfoPtr &run_info,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						returnSchedulerActiveTask(
											const int task_svr_id, 
											const u64 &task_docid,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						recoverSchedulerActiveTask(
											const int task_svr_id, 
											const u64 &task_docid,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						addErrorMsg(
											const AosXmlTagPtr &job_doc,
											const OmnString &error_msg,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						initJob(
											const AosJobMgrObj::OprId id,
											const AosXmlTagPtr &jobdoc,
											const AosXmlTagPtr &sdoc, 
											const AosRundataPtr &rdata) = 0;
	virtual bool 						datacollectorFinished(
											const AosRundataPtr &rdata, 
											const OmnString &datacol_id, 
											vector<OmnString> &logic_taskid) = 0;
	virtual bool 						setRunTimeValue(
											const OmnString &key,
											const AosValueRslt &value,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						taskFailed(
											const AosXmlTagPtr &task_doc,
											const u64 &task_stime,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						isValid(
											const u64 &task_docid,
											const u64 &stime,
											const AosRundataPtr &rdata) = 0;
	virtual u64 						getJobDocid() const = 0;
	virtual OmnString 					getTaskContainer() const = 0;
	virtual OmnString 					getLogContainer() const = 0;
	virtual OmnString 					getJobObjid() const = 0;
	virtual OmnString 					getJobENV(
											const OmnString &key) = 0;
	virtual AosRundataPtr 				getRundata() const = 0;
	virtual AosXmlTagPtr 				getJobDoc() const = 0;
	virtual AosXmlTagPtr 				getSnapShots(
											const AosRundataPtr &rdata) = 0;
	virtual map<OmnString, OmnString>& 	getJobENV() = 0;
	virtual AosDataCollectorMgrPtr 		getDataColMgr() = 0;
	virtual AosJobObjPtr 				create(
											const AosJobMgrObj::OprId id,
											const AosXmlTagPtr &jobdoc,
											const AosXmlTagPtr &sdoc,
											const AosRundataPtr &rdata) const = 0;
	virtual AosLogicTaskObjPtr 			getLogicTask(
											const OmnString &logic_id,
											const AosRundataPtr &rdata) = 0;
	virtual AosLogicTaskObjPtr 			getLogicTaskLocked(
											const OmnString &logic_id,
											const AosRundataPtr &rdata) = 0;
	virtual AosValueRslt 				getRunTimeValue(
											const OmnString &key,
											const AosRundataPtr &rdata) = 0;
};

#endif

