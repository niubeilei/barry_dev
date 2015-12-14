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
// 2013/01/06 Creaed by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AppIdGens/Testers/AppIdGenU64Tester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Random/random_util.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool 
AosAppIdGenU64Tester::start()
{
	cout << "    Start AosAppIdGenU64 Tester ...";
	return basicTest();
}


bool 
AosAppIdGenU64Tester::basicTest()
{
	// It randomly creates an ID gen and runs it.
	int tries = 100;
	while (tries--)
	{
	}
	return false;
}

