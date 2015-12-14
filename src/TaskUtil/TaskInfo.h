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
#ifndef AOS_TaskUtil_TaskInfo_h
#define AOS_TaskUtil_TaskInfo_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskType.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskUtil/TaskHeartbeat.h"
#include "TaskUtil/TaskStartType.h"
#include "TaskUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosTaskInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

	u64					mTaskDocid;
	AosTaskType::E		mTaskType;
	AosTaskStatus::E	mStatus;
	OmnString			mTaskId;
	int					mOptimalPhyId;
	int					mProgress;
	int 				mTaskServerId;
	u64					mTaskStartStampTime;
	OmnString			mLogicId;
	int					mPriority;
	u64					mUpdateEndTime;
	bool				mIsNeedSnapShot;
	
public:
	AosTaskInfo();
	~AosTaskInfo();

	u64			getTaskDocid() const {return mTaskDocid;}
	AosTaskType::E getTaskType() const {return mTaskType;}
	OmnString	getTaskId() const {return mTaskId;}
	int			getOptimalPhyId() const {return mOptimalPhyId;}
	void		setOptimalPhyId(const int id)  {mOptimalPhyId = id;}
	int			getTaskServerId() const {return mTaskServerId;}
	void		setTaskServerId(const int id)  {mTaskServerId = id;}
	OmnString	getLogicId(){return mLogicId;}

	AosTaskStatus::E getStatus() const {return mStatus;}
	void		setStatus(const AosTaskStatus::E st) {mStatus = st;}

	void		setTaskStartSTime(const u64 &time) {mTaskStartStampTime = time;}
	u64			getTaskStartSTime() {return mTaskStartStampTime;}

	int 		getPriority(){return mPriority;}
	void		incrementPriority(){mPriority ++;}

	int			getProgress() const {return mProgress;}
	void		setProgress(const int p) {mProgress = p;}

	u64 		getUpdateEndTime() const {return mUpdateEndTime;}
	void		setUpdateEndTime(const u64 &v) {mUpdateEndTime = v;}
	bool		needSnapShot() const {return mIsNeedSnapShot;}

	static AosTaskInfoPtr	create(const AosTaskObjPtr &task);

	static AosTaskInfoPtr 	create(const AosXmlTagPtr &task_doc);

};


struct AosTaskInfoCompareFun
{
	int 				mOptimalPhyId;
	AosTaskType::E      mTaskType;


	AosTaskInfoCompareFun(const int i, const AosTaskType::E task_type)
	:
	mOptimalPhyId(i),
	mTaskType(task_type)
	{
	}

	bool operator() (const AosTaskInfoPtr &lhs, const AosTaskInfoPtr &rhs)
	{
		if (lhs->getPriority() !=  rhs->getPriority())
		{
			return lhs->getPriority() > rhs->getPriority();
		}
		if (lhs->getOptimalPhyId() == mOptimalPhyId && rhs->getOptimalPhyId() == mOptimalPhyId)
		{
			//return lhs->getTaskType() < rhs->getTaskType();
			if(lhs->getTaskType() == mTaskType && rhs->getTaskType() == mTaskType)
			{
				return lhs->getTaskDocid() < rhs->getTaskDocid();
				//return lhs->getPriority() > rhs->getPriority();
			}

			if(lhs->getTaskType() == mTaskType && rhs->getTaskType() != mTaskType) return true;

			if(lhs->getTaskType() != mTaskType && rhs->getTaskType() == mTaskType) return false;
		}

		if (lhs->getOptimalPhyId() == mOptimalPhyId && rhs->getOptimalPhyId() != mOptimalPhyId)
		{
			return true;
		}

		if (lhs->getOptimalPhyId() != mOptimalPhyId && rhs->getOptimalPhyId() == mOptimalPhyId)
		{
			return false;
		}

		if (lhs->getOptimalPhyId() == rhs->getOptimalPhyId())
		{
			//return lhs->getTaskType() < rhs->getTaskType();
			if (lhs->getTaskType() == rhs->getTaskType())
			{
				//return lhs->getPriority() > rhs->getPriority();
				return lhs->getTaskDocid() < rhs->getTaskDocid();
			}
			return lhs->getTaskType() < rhs->getTaskType();
		}
		return lhs->getOptimalPhyId() < rhs->getOptimalPhyId();
	}
};


#endif

