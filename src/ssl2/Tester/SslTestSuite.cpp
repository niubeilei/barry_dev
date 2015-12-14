////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl2/Tester/SslTestSuite.h"

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
#include "ssl2/Tester/RecordTester.h"
#include "ssl2/Tester/ClientTester.h"
#include "ssl2/Tester/ServerTester.h"
#include "ssl2/Tester/Jns9054Tester.h"

extern bool AosSSLTesterIsServerFlag;
extern bool AosSSLTesterIsClientFlag;


OmnTestSuitePtr		
AosSslTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("SslTestSuite", "SSL Test Suite");

    // suite->addTestPkg(OmnNew AosSslRecordTester());

	// 
	// Now add all the testers
	//
	if (AosSSLTesterIsServerFlag)
	{
     	suite->addTestPkg(OmnNew AosSslServerTester());
	}
 
 	if (AosSSLTesterIsClientFlag)
 	{
     	suite->addTestPkg(OmnNew AosSslClientTester());
	}

    // suite->addTestPkg(OmnNew AosJns9054Tester());

	return suite;
}

