////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Jns9054Tester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl/Tester/Jns9054Tester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool AosJns9054Tester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosJns9054Tester::basicTest()
{
	OmnBeginTest << "Test JNS 9054 Card";
	mTcNameRoot = "Test_JNS9054";

	int i = 10;
	OmnTC(OmnExpected<int>(11), OmnActual<int>(i)) 
		<< "This is a test on integer: " << i << endtc;

	return true;
}	

