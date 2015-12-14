////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemoryTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/MemoryTester.h"

#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/Memory.h"



bool AosMemoryTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	testKernel();
	testLeak();
	return true;
}


bool AosMemoryTester::basicTest()
{
	OmnBeginTest << "Test AosMemory";
	mTcNameRoot = "MemoryTest";
	
	char errmsg[99+1];
	int ret = AosMemory_init(errmsg);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	ret = AosMemory_setStatus();
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	void *ptr = aos_malloc(999+1);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptr != 0)) << endtc;

	ret = aos_free(ptr);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	return true;
}


bool AosMemoryTester::testKernel()
{
	// int ret;

	OmnBeginTest << "Test AosMemory Kernel";
	mTcNameRoot = "KernelMemoryTest";

	// OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	return true;
}


bool AosMemoryTester::testLeak()
{
	OmnBeginTest << "Test AosMemory Leak";
	mTcNameRoot = "MemoryLeakTest";

	int i, ret;
	const int num_tries = 99+1;
	void *ptrs[num_tries];
	u32 size = 999+1;

	// 
	// Allocate
	//
	for (i=0; i<num_tries; i++)
	{
		ptrs[i] = aos_malloc(size);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptrs[i] != 0)) << endtc;
	}

	for (i=0; i<num_tries; i++)
	{
		ret = AosMemory_checkMemory((char *)ptrs[i]);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}

	// 
	// Free 
	//
	for (i=0; i<num_tries; i++)
	{
		ret = aos_free((char *)ptrs[i]);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}

	for (i=0; i<num_tries; i++)
	{
		ret = AosMemory_checkMemory((char *)ptrs[i]);
		OmnTC(OmnExpected<int>(-eAosRc_EntryNotFound), OmnActual<int>(ret)) << endtc;
	}

	return true;
}

