////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ForwardTableTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProc/http_proc/Tester/ForwardTableTester.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool AosForwardTableTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosForwardTableTester::basicTest()
{
	//int ret;
	OmnBeginTest << "Test Forward Table CLI";
	mTcNameRoot = "Fwdtbl_CLI";

	// 
	// Clear all
	//
	aosRunCli("forward table entry add app1 1234 192.168.1.20 200 ssl", true);
	
//	for (int i=0; i<5000; i++)
//	{
//		char local[200];
//		sprintf(local, "forward table entry add vs1 ww%d.test.com 192.168.1.20 5000 ssl", i);
//		aosRunCli(local, true);
//	}

	return true;
}

