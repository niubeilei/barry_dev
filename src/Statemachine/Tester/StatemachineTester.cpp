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
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Statemachine/Tester/StatemachineTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Statemachine/Statemachine.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool 
AosStatemachineTester::start()
{
	cout << "    Start OmnString Tester ...";
	basicTest();
	return true;
}


bool 
AosStatemachineTester::basicTest()
{
	OmnBeginTest << "Test OmnString(const char *d)";
	OmnCreateTc << (c1.length() == 0) << endtc;
	return true;
}

