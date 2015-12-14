////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringTester.cpp
// Description:
//   
//
// Modification History:
// 11/21/2012 Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/SPtrTester.h"

#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util1/Time.h"



bool AosSPtrTester::start()
{
	cout << "    Start SPtr Tester ...";

	basicTest();
	return true;
}


bool
AosSPtrTester::basicTest()
{
	int count = 1000 * 1000 * 20;
	AosBuffPtr buff1 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosBuffPtr buff2 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	u64 start = OmnGetTimestamp();

	// Step 1: test the assignment operator
	OmnScreen << "Test assignment" << endl;
	for (int i=0; i<count; i++)
	{
		if (i % 10000000 == 0)
		{
			u64 delta = OmnGetTimestamp() - start;
			if (i > 0 && delta > 0)
			{
				cout << "Total Operations: " << i/1000000 << " millions. Time: " 
					<< delta/1000.0 << " (ms). Average per operation: " 
					<< delta * 1.0 / i   << endl;
			}
		}

		AosBuffPtr bb = buff1;
		buff1 = buff2;
		buff2 = bb;
	}

	// Step 2: test the -> operator
	OmnScreen << "Test operator ->" << endl;
	start = OmnGetTimestamp();
	for (int i=0; i<count; i++)
	{
		if (i % 10000000 == 0)
		{
			u64 delta = OmnGetTimestamp() - start;
			if (i > 0 && delta > 0)
			{
				cout << "Total Operations: " << i/1000000 << " millions. Time: " 
					<< delta/1000.0 << " (ms). Average per operation: " 
					<< delta * 1.0 / i   << endl;
			}
		}

		int len = buff1->dataLen();
		len = 0;
	}

	return true;
}


