////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgrTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrTester/AppMgrTester.h"

#include "AppMgr/AppMgr.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

OmnAppMgrTester::OmnAppMgrTester()
{
	mName = "OmnAppMgrTester";
}


OmnAppMgrTester::~OmnAppMgrTester()
{
}


bool 
OmnAppMgrTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "AppMgr - Application Management Function Tester";
	testDefaultConstructor();
	testIsConnected();

	return true;
}


bool
OmnAppMgrTester::testDefaultConstructor()
{
	mTcNameRoot = "AppMgrDltCtor";

	// 
	// 
	return true;
}


bool
OmnAppMgrTester::testIsConnected()
{
	mTcNameRoot = "IsConnected";

	// 
	// Need to make sure the AppMgr server is actually running. 
	//
	OmnCreateTc << OmnExpected<int>(true) 
		<< OmnActual<bool>(OmnAppMgrSelf->isConnected()) << endtc;

	return true;
}
