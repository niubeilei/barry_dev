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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/Tester/DataCommTester.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;

AosDataCommTester::AosDataCommTester()
{
	mName = "DataCommTester";
	mTries = 100;
}


bool 
AosDataCommTester::start()
{
	return basicTest();
}


bool
AosDataCommTester::basicTest()
{
	if (mTestDurationSec > 0)
	{
		// Run the torturer for 'mTestDuration' amount of time.
		i64 crt_sec = OmnTime::getCrtSec();
		while (OmnTime::getCrtSec() < crt_sec + mTestDurationSec)
		{
			runOneTest(mBatchSize);
		}
	}
	else
	{
		if (mTries <= 0) mTries = 100;
		for (int i=0; i<mTries; i+=mBatchSize)
		{
			runOneTest(mBatchSize);
		}
	}

	return true;
}


bool
AosDataCommTester::runOneTest(const int batch_size)
{
	for (int i=0; i<batch_size; i++)
	{
		switch (rand() % 4)
		{
		case 0:
			 //testCreateData(...);
			 break;

		case 1:
			 //testModifyData(...);
			 break;

		}
	}
}



