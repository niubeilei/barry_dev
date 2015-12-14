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
#if 0
#ifndef AOS_TaskMgr_TaskScheduler_h
#define AOS_TaskMgr_TaskScheduler_h

#include "Rundata/Rundata.h"
#include "TaskMgr/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <queue>
using namespace std;


class AosTaskScheduler : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	AosSmartDocObjPtr		mReqNormalizer;
	queue<AosTaskObjPtr>	mPendingTasks;

public:
	AosTaskScheduler();
	~AosTaskScheduler();

	// ThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool start();
	bool stop();
	bool config(const AosXmlTagPtr &conf);
	bool addRequest(const AosXmlTagPtr &req, const AosRundataPtr &rdata);

private:
	bool scheduleTasks();
};
#endif
#endif
