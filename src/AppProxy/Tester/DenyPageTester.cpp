////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DenyPageTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProxy/Tester/DenyPageTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


bool AosDenyPageTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosDenyPageTester::basicTest()
{
	OmnBeginTest << "Test SSL Client";
	mTcNameRoot = "Test_SSLClient";
	return true;
}	

