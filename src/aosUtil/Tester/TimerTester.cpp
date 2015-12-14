////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/TimerTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Porting/Sleep.h"

#include "aosUtil/Timer.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/jiffies.h"


static u32 sgFlag = 0;

static int AosTimerTestCallback(unsigned long data)
{
	//sgFlag = jiffies;
	//cout << "call back again:%lu\n " << sgFlag << endl;
	//AosTimer_t *timer = AosTimer_addTimer(200, 0, AosTimerTestCallback);
	return 0;
}


bool AosTimerTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosTimerTester::basicTest()
{
	OmnBeginTest << "Test AosSlab";
	mTcNameRoot = "TimerTest";

	//
	// Test the addTimer 
	// 
	u32 currentJiffies = jiffies;
	AosTimer_t *timer = AosTimer_addTimer(200, 0, AosTimerTestCallback);
	OmnSleep(1);
	u32 newJiffies = sgFlag;

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(newJiffies - currentJiffies >= 200)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(newJiffies - currentJiffies < 300)) << endtc;
	
	//
	// Test the modTimer 
	// 
	int ret1 = AosTimer_modTimer(timer, 2000);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret1)) << endtc;
	
	//
	// Test the delTimer 
	// 
	int ret2 = AosTimer_delTimer(timer);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret2)) << endtc;

	return true;
}


