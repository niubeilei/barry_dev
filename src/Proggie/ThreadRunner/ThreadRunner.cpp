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
// 	This class will start one or more threads. The thread function 
// 	simply calls the AosThreadRunnerObj::doThreadFunc(...). This is a
// 	convenient class. One can use this one to start multiple threads. 
//
// Modification History:
// 03/24/2009 	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Proggie/ThreadRunner/ThreadRunner.h"

#include "alarm_c/alarm.h"
#include "Thread/Thread.h"
#include "Proggie/ThreadRunner/ThreadRunnerObj.h"
#include "Util/OmnNew.h"


AosThreadRunner::AosThreadRunner(const int numThreads, 
		const AosThreadRunnerObjPtr &proc)
:
mNumThreads(numThreads),
mProcessor(proc),
mThreadFunc(0),
mUserdata(0)
{
	aos_assert(numThreads > 0);
	aos_assert(mProcessor);
	aos_assert(start());
}


AosThreadRunner::AosThreadRunner(const int numThreads, 
		const AosThreadRunnerFunc func, 
		void *userdata)
:
mNumThreads(numThreads),
mThreadFunc(func),
mUserdata(userdata)
{
	aos_assert(numThreads > 0);
	aos_assert(mProcessor);
	aos_assert(start());
}


AosThreadRunner::~AosThreadRunner()
{
}


bool
AosThreadRunner::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	OmnThreadPtr thrd;
	for (int i=0; i<mNumThreads; i++)
	{
		thrd = OmnNew OmnThread(thisPtr, "ThreadRunner", i, true, false, __FILE__, __LINE__);
		thrd->start();
		aos_assert_r(thrd, false);
		mThreads.push_back(thrd);
	}
	return true;
}


bool    
AosThreadRunner::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		if (mProcessor)
		{
			mProcessor->doThreadFunc(thread);
		}
		else
		{
			aos_assert_r(mThreadFunc, false);
			mThreadFunc(thread, mUserdata);
		}
	}
	return true;
}


bool    
AosThreadRunner::signal(const int thrd_id)
{
	return true;
}

void    
AosThreadRunner::heartbeat(const int tid)
{
}


bool    
AosThreadRunner::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

