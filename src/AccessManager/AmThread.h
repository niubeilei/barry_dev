////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 4/5/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_AccessMgr_AmThread_h
#define Aos_AccessMgr_AmThread_h

#include "AccessManager/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosAmThread : public OmnThreadedObj
{
	OmnDefineRCObject;

private: 
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	AosAmTransPtr	mTrans;
	AosAccessMgrPtr	mAccessMgr;
	OmnThreadPtr	mThread;

public:
	AosAmThread(const AosAccessMgrPtr &accessMgrPtr);
	~AosAmThread();

	// 
	// ThreadedObj interface
	//
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	process(const AosAmTransPtr &trans);

	// inner cover of process(const AosAmTransPtr &trans)
	bool	procTrans();

private:
};
#endif
#endif
