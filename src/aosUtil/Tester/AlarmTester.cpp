////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AlarmTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/AlarmTester.h"

#include "aosUtil/Alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosAlarmTester::AosAlarmTester()
{
	mName = "AosAlarmTester";
}


AosAlarmTester::~AosAlarmTester()
{
}
 
 
bool 
AosAlarmTester::start()
{
	OmnBeginTest << "AosAlarm Tester";

	aos_alarm_init();
	normalTests();
	abnormalTests();
	return true;
}


bool
AosAlarmTester::normalTests()
{
	// 
	// It tests the normal functions.
	//
	int ret;
	ret = aos_warn(eAosMD_SSL, eAosAlarm_ProgErr, 
		"This is a warn. Int = %d, msg: %s\n", 15, "this is a test");
	OmnTC(OmnExpected<int>(-eAosAlarm_ProgErr), OmnActual<int>(ret)) << endtc;	

	ret = aos_alarm(eAosMD_CLI, eAosAlarm_ProgErr, 
		"This is an alarm. Int = %d, msg: %s\n", 15, "this is a test");
	OmnTC(OmnExpected<int>(-eAosAlarm_ProgErr), OmnActual<int>(ret)) << endtc;	

	ret = aos_progerr(eAosMD_DenyPage, eAosAlarm_ProgErr, 
		"This is a progerr. Int = %d, msg: %s\n", 15, "this is a test");
	OmnTC(OmnExpected<int>(-eAosAlarm_ProgErr), OmnActual<int>(ret)) << endtc;	

	ret = aos_fatal(eAosMD_AppProc, eAosAlarm_ProgErr, 
		"This is a fatal. Int = %d, msg: %s\n", 15, "this is a test");
	OmnTC(OmnExpected<int>(-eAosAlarm_ProgErr), OmnActual<int>(ret)) << endtc;	

	return true;
}
  

bool
AosAlarmTester::abnormalTests()
{
	int ret;

	// 
	// Use a undefined alarm module
	//
	ret = aos_warn(20202020, eAosAlarm_ProgErr, ("Undefined alarm module."));
	OmnTC(OmnExpected<int>(-eAosAlarm_ProgErr), OmnActual<int>(ret)) << endtc;	

	return true;
}



