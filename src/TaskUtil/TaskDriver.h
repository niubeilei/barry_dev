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
//	
// 06/18/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskUtil_TaskDriver_h
#define AOS_TaskUtil_TaskDriver_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "SEInterfaces/TransType.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "TaskUtil/Ptrs.h"
#include <queue>

OmnDefineSingletonClass(AosTaskDriverSingleton,
						AosTaskDriver,
						AosTaskDriverSelf,
						OmnSingletonObjId::eTaskDriver,
						"TaskDriver");

class AosTaskDriver: virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

	enum 
	{
		eDefaultHbTimerSec = 1 * 60
	};

public:
	enum EventTrigger 
	{
		eInvalid,
		eUpdateTaskProgress,
		eUpdateTaskStatus,
		eScheduleNextTask,
		eSignalHeartbeat,
		eNotifyJobTaskStop,
		eSvrDeath,
		eStopJob,
		eMax
	};

private:
	int					mHbTimerSec;
	OmnMutexPtr         mLock;
	OmnCondVarPtr       mCondVar;
	OmnThreadPtr        mThread;
	queue<AosTaskDriverRequestPtr> 	mQueue; 
	int					mHbCount;

public:
	AosTaskDriver();
	~AosTaskDriver();

	// Singleton class interface
    static AosTaskDriver * getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);


	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	addEvent(const AosTaskDriverRequestPtr &req);
	bool	scheduleTasks();
	bool	addSvrDeathEvent(const int svr_id);
	bool	stopJob(const u64 &job_docid, const AosRundataPtr &rdata);

	int		getHbTimerSec() {return mHbTimerSec;}

private:
	bool	updateTaskProgress(const AosTaskDriverRequestPtr &req);
	bool	updateTaskStatus(const AosTaskDriverRequestPtr &req);
	bool	scheduleNextTask(const AosTaskDriverRequestPtr &req);
	bool 	getTaskRunnerInfoBySvrId(const AosTaskDriverRequestPtr &req);
	AosJobObjPtr getStartJob(const AosTaskRunnerInfoPtr &runner_info,  const AosRundataPtr &rdata);
	bool	stopJobPriv(const AosTaskDriverRequestPtr &req);
};

#endif
