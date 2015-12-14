////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RwlockTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/RwlockTester.h"

#include "Debug/Debug.h"
#include "KernelAPI/UserLandWrapper/RwlockU.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/Rwlock.h"



bool AosRwlockTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	testKernel();
	return true;
}


bool AosRwlockTester::basicTest()
{
	bool ret;

	OmnBeginTest << "Test AosRwlock";
	mTcNameRoot = "RwlockTest";

	// 
	//
	AOS_DECLARE_RWLOCK(mylock);
	AOS_INIT_RWLOCK(mylock);

	ret = aos_read_lock(mylock);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;

	aos_read_unlock(mylock);


	return true;
}


bool AosRwlockTester::testKernel()
{
	int ret;

	OmnBeginTest << "Test AosRwlock Kernel";
	mTcNameRoot = "KernelRwlockTest";

OmnTrace << "To test rwlock " << endl;

	ret = aosRwlockWrapper();
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	return true;
}



