///////////////////////////////////////////////////////////////////////////
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
//
// Modification History:
// 04/29/2012: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TaskMgr_TaskThrd_h
#define Aos_TaskMgr_TaskThrd_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/MetaExtension.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Buff.h" 

#include <deque>
using namespace std;

class AosTaskThrd : virtual public OmnThreadedObj
{
	OmnDefineRCObject;
	
private:
	enum Status
	{
		eStop,
		eStart,
		eFinish,
		eError
	};

	OmnMutexPtr        	mLock;
	OmnCondVarPtr      	mCondVar;
	u32					mThreadId;
	OmnThreadPtr		mThread;
	Status				mStatus;
	u64					mTotalTasksProced;
	u64 				mTaskDocid;
	int					mProgress;
	OmnString			mCheckedTime;
	int					mRunNum;

public:
	AosTaskThrd(const u64 task_docid, const AosRundataPtr &rdata);
	~AosTaskThrd();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual void	heartbeat(const int thrdLogicId);
	virtual bool    signal(const int threadLogicId);
	bool	runTask(const AosTaskObjPtr &task, const AosRundataPtr &rdata);
	inline bool isFinished() const 
	{
		return mStatus == eFinish || mStatus == eError;
	}
	void clear();

	void registerTaskThrd();
	static AosTaskThrd* findTaskThrd(u64 taskDocId);
};
#endif
