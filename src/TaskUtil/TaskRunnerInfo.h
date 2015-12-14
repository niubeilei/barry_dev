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
#ifndef AOS_TaskUtil_TaskRunnerInfo_h
#define AOS_TaskUtil_TaskRunnerInfo_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskType.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskMgr/TaskErrorType.h"
#include "TaskUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "XmlUtil/Ptrs.h"


class AosTaskRunnerInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

	u64						mJobDocid;
	OmnString				mLogicId;
	u64						mTaskDocid;
	OmnString				mTaskType;
	AosTaskStatus::E		mStatus;
	int						mTaskRunServerId;
	u64						mTaskStartStampTime;
	int						mProgress;
	AosXmlTagPtr 			mTaskDoc;
	bool					mIdle;
	AosTaskErrorType::E 	mErrorType;
	OmnString				mErrmsg;
	OmnString				mOutPuts;
	OmnString				mProcNum;
	OmnString				mSnapShots;
	
public:
	AosTaskRunnerInfo();

	AosTaskRunnerInfo(
			const u64 &job_docid,
			const int task_svr_id,
			const bool idle);

	AosTaskRunnerInfo(
		const u64 &job_docid,
		const OmnString &logic_id,
		const OmnString &task_type,
		const u64 &task_docid,
		const int task_svr_id,
		const AosTaskStatus::E sts,
		const int progress,
		const u64 &timestamp,
		const AosTaskErrorType::E &error_type,
		const OmnString &errmsg);

	~AosTaskRunnerInfo();

	bool serializeFrom(const AosBuffPtr &buff);

	bool serializeTo(const AosBuffPtr &buff);

	OmnString toString();

	bool		isIdle()const {return mIdle;}

	u64			getTaskDocid() const {return mTaskDocid;}
	int			getTaskRunServerId() const {return mTaskRunServerId;}

	AosTaskStatus::E getStatus() const {return mStatus;}

	OmnString 	getLogicId() const {return mLogicId;}
	u64 		getJobDocid() const {return mJobDocid;}

	u64 		getTaskStartSTime() const {return mTaskStartStampTime;}

	int			getProgress() const {return mProgress;}
	void 		setProgress(const int p) {mProgress = p;}
	AosTaskErrorType::E getErrorType()const {return mErrorType;}
	OmnString			getErrmsg() const {return mErrmsg;}

	bool		setTaskOutPuts(const vector<AosXmlTagPtr> &task_outputs);
	OmnString	getTaskOutPuts();

	bool		setTaskProcNum(const AosXmlTagPtr &xml);
	OmnString	getTaskProcNum() { return mProcNum; }

	bool		setTaskSnapShots(const AosXmlTagPtr &snapshots);
	OmnString	getTaskSnapShots();
};

#endif

