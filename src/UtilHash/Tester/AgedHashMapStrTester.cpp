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
// 01/20/2012:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/Tester/AgedHashMapStrTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "UtilHash/AgedHashMapStr.h"


bool AosAgedHashMapStrTester::start()
{
	// Test default constructor
	cout << "    Start AosStrHash Tester...";
	basicTest();
	return true;
}


bool 
AosAgedHashMapStrTester::basicTest()
{
	OmnBeginTest << "Test";

	AosAgedHashMapStr<int, 100> hash;

	const int tries = 10000;
	for (int i=0; i<tries; i++)
	{
	}

	return true;
}

