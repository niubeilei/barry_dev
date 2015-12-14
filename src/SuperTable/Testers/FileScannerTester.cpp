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
// 2013/10/29:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SuperTable/Testers/FileScannerTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <map>


AosFileScannerTester::AosFileScannerTester() 
{
}


bool AosFileScannerTester::start()
{
	// Test default constructor
	cout << "    Start AosFileScanner Tester..." << endl;
	basic();
	return true;
}


bool 
AosFileScannerTester::basic()
{
	OmnBeginTest << "Test";

	// int rounds = 1;
	// for (int i=0; i<rounds; i++)
	// {
	// 	aos_assert_r(testOneRound(), false);
	// }
	return true;
}


bool    
AosFileScannerTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1);
	}
	return true;
}


bool    
AosFileScannerTester::signal(const int threadLogicId)
{
	return true;
}


