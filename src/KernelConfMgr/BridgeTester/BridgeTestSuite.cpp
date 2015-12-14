////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BridgeTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SystemCli/BridgeTester/BridgeTestSuite.h"

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

#include "SystemCli/BridgeTester/BridgeTester.h"

OmnTestSuitePtr		
AosBridgeTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("BridgeTestSuite", "Bridge Test Suite");


	// 
	// Now add all the testers
	//
    suite->addTestPkg(OmnNew AosBridgeTester());
 
	return suite;
}

AosBridgeTestSuite::AosBridgeTestSuite()
{
}

AosBridgeTestSuite::~AosBridgeTestSuite()
{
}



