////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MutexTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/MutexTester.h"

#include "Debug/Debug.h"
#include "KernelAPI/UserLandWrapper/MutexU.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/Mutex.h"



bool AosMutexTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	testKernel();
	return true;
}


bool AosMutexTester::basicTest()
{
	bool ret;

	OmnBeginTest << "Test AosMutex";
	mTcNameRoot = "MutexTest";

	// 
	//
	AOS_DECLARE_MUTEX(mylock);
	AOS_INIT_MUTEX(mylock);

	ret = aos_lock(mylock);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;

	aos_unlock(mylock);


	return true;
}


bool AosMutexTester::testKernel()
{
	int ret;

	OmnBeginTest << "Test AosMutex Kernel";
	mTcNameRoot = "KernelMutexTest";

	ret = aosMutexWrapper();
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	return true;
}



