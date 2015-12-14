////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelTester/SslTester.h"


#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool AosSslTester::start()
{
	// 
	// Test default constructor
	//
	normalTest();
	return true;
}


bool AosSslTester::normalTest()
{
	OmnBeginTest << "Test Ssl";
	mTcNameRoot = "Test_Normal";
//	int ret;

//	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}

	
