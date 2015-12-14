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
// 	This is an interface used for 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Proggie_ThreadRunner_ThreadRunner_h
#define Omn_Proggie_ThreadRunner_ThreadRunner_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include <list>


typedef void *(*AosThreadRunnerFunc)(const OmnThreadPtr &thread, void *);

class AosThreadRunner : public OmnThreadedObj
{
private:
	OmnDefineRCObject;

	int						mNumThreads;
	AosThreadRunnerObjPtr	mProcessor;
	AosThreadRunnerFunc		mThreadFunc;
	std::list<OmnThreadPtr>	mThreads;
	int						mMaxProcTime;
	bool					mCheckThread;
	int						mCheckThreadIntervalSec;
	void *					mUserdata;

public:
	AosThreadRunner(const int numThreads, const AosThreadRunnerObjPtr &processor);
	AosThreadRunner(const int numThreads, 
			const AosThreadRunnerFunc func, 
			void *userdata);
	~AosThreadRunner();

	bool	start();
	void	setCheckThread(const bool c) {mCheckThread = c;}
	void	setMaxProcTime(const int t) {mMaxProcTime = t;}

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual void    heartbeat(const int tid);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

};
#endif

