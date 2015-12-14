//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 03/24/2009	Created by Chen Ding
//////////////////////////////////////////////////////////////////////////
#include "Thread/Tester/ThreadRunnerTester.h"

#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Porting/ThreadDef.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/ThrdMsgQueue.h"
#include "Thread/ThreadShellMgr.h"
#include "Thread/ThreadRunner.h"
#include "Thread/Tester/Ptrs.h"
#include "Util/OmnNew.h"


AosThreadRunnerTester::AosThreadRunnerTester()
{
    mTries = 10000;
    mName = "AosThreadRunnerTester";
}


AosThreadRunnerTester::~AosThreadRunnerTester()
{
}


bool
AosThreadRunnerTester::start()
{
    basicTest();
    return true;
}


void *AosThreadRunnerTester_threadFunc(const OmnThreadPtr &thread, void *ud)
{
	while (1)
	{
		cout << "In func thread: " << thread->getThreadId() 
			<< ". Userdata: " << (int)ud << endl;
		int sec = rand()%5;
		OmnSleep(sec);
	}
}


bool
AosThreadRunnerTester::basicTest()
{
	AosThreadRunnerObjPtr thisPtr(this, false);
	mThreadRunner = OmnNew AosThreadRunner(300, thisPtr);
	mThreadRunner->start();
	OmnSleep(20);

	mThreadRunner = OmnNew AosThreadRunner(250, AosThreadRunnerTester_threadFunc, 
			(void *)123);
	mThreadRunner->start();
	OmnSleep(20);
	return true;
}


bool
AosThreadRunnerTester::doThreadFunc(const OmnThreadPtr &thread)
{
	while (1)
	{
		cout << "In thread: " << OmnGetCurrentThreadId() << endl;
		int sec = rand()%5;
		OmnSleep(sec);
	}
	return true;
}


