////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppManagementTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelAPITester/KernelAPIAppMgmtTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

OmnKernelAPIAppMgmtTester::OmnKernelAPIAppMgmtTester()
{
	mName = "OmnKernelAPIAppMgmtTester";
}


OmnKernelAPIAppMgmtTester::~OmnKernelAPIAppMgmtTester()
{
}


bool 
OmnKernelAPIAppMgmtTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "KernelAPI - Application Management Function Tester";
	return true;
}


bool
OmnKernelAPIAppMgmtTester::testAppExit()
{
	mTcNameRoot = "AppExitAPI";

	// 
	// 1. Register for application stop
	// 2. Start the application
	// 3. The application stops
	// 4. This function should receive the event
	// 5. The event contains the correct information
	// 
	return true;
}
