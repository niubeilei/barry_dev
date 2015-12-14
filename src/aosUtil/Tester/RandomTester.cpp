////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/RandomTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/Random.h"



bool AosRandomTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosRandomTester::basicTest()
{
	OmnBeginTest << "Test AosRandom";
	mTcNameRoot = "RandomTest";
	const u32 len = 999+1;
	char local[len];
	u32 strlen;

	// 
	// Abnormal Condition: string length = 0
	//
	strlen = 0;
	memset(local, 0, len);
	AosRandom_get(local, strlen);
	check(__FILE__, __LINE__, local, strlen, len);

	// 
	// String length = 1
	//
	strlen = 1;
	memset(local, 0, len);
	AosRandom_get(local, strlen);
	check(__FILE__, __LINE__, local, strlen, len);

	// 
	// String length = 2
	//
	strlen = 2;
	memset(local, 0, len);
	AosRandom_get(local, strlen);
	check(__FILE__, __LINE__, local, strlen, len);

	// 
	// String length = 99+1 
	//
	strlen = 99=1;
	memset(local, 0, len);
	AosRandom_get(local, strlen);
	check(__FILE__, __LINE__, local, strlen, len);

	// 
	// String length = 999+1 
	//
	strlen = 999+1;
	memset(local, 0, len);
	AosRandom_get(local, strlen);
	check(__FILE__, __LINE__, local, strlen, len);

	return true;
}


bool AosRandomTester::check(
		char *file, 
		int line, 
		char *buff, 
		const u32 start, 
		const u32 end)
{
	u32 i;
	for (i=0; i<start; i++)
	{
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(buff[i] != 0)) 
			<< "Called from: " << file << ":" << line << endtc;
	}


	for (i=start; i<end; i++)
	{
		OmnTC(OmnExpected<int>(0), OmnActual<int>(buff[i])) 
			<< "Called from: " << file << ":" << line << endtc;
	}

	return true;
}

