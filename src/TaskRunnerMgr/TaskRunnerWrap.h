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
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskRunnerWrap_TaskRunnerWrap_h
#define AOS_TaskRunnerWrap_TaskRunnerWrap_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "TaskMgr/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadedObj.h" 

class AosTaskRunnerWrap :  public OmnRCObject  
{
	OmnDefineRCObject;

private:
	enum Status
	{
		eInvalid,

		eIdle,
		eRunning,
		eFailed,
		ePaused,
		eAborted,
		eSuccess,

		eMax
	};
	Status				mStatus;

	AosRundataPtr mRunData;
	pid_t					mPid;
	int 					mReadFd;
	int 					mWriteFd;
	u32						mHeartBeatTime;
	u32						mTaskProcTime;
	AosBuffPtr				mTask;

public:
	AosTaskRunnerWrap(pid_t &pid, int &fd0 , int &fd1);
	~AosTaskRunnerWrap()
	{
	}

	int readMsg();
	bool kill();
	bool taskRunnerFailed();
	bool addTask(const AosBuffPtr &task);
	AosRundataPtr getRdata(){return mRunData;}
	int getReadFd();
	bool checkValid();
	bool cleanResource();
	bool isFinished(){return isFinished(mStatus); }
	inline static bool isFinished(const Status code)
	{
		return code == eFailed || code == eAborted || code == eSuccess;
	}
	AosBuffPtr getTask() const;

};
#endif

