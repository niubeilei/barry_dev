////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMgrTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SystemCli/MemMgrTester/MemMgrTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/TrafficGenThread.h"

#include "SystemCli/MemMgrTester/MemMgrTester.h"

OmnTestSuitePtr		
AosMemMgrTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("MemMgrTestSuite", "MemMgr Test Suite");


	// 
	// Now add all the testers
	//
    suite->addTestPkg(OmnNew AosMemMgrTester());
 
	return suite;
}

AosMemMgrTestSuite::AosMemMgrTestSuite()
{
}

AosMemMgrTestSuite::~AosMemMgrTestSuite()
{
}



