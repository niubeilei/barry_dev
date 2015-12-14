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


#include "InetCtrl/Tester/InetTestSuite.h"
#include "InetCtrl/Tester/InetCliTester.h"
#include "InetCtrl/Tester/IacTester.h"

OmnTestSuitePtr		
AosInetTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("InetTestSuite", "Inet Test Suite");

	// 
	// Now add all the testers
	//
//    suite->addTestPkg(OmnNew AosInetCliTester());
//    suite->addTestPkg(OmnNew AosTrafficTester());
      suite->addTestPkg(OmnNew AosIacTester());
 
	return suite;
}

AosInetTestSuite::AosInetTestSuite()
{
}

AosInetTestSuite::~AosInetTestSuite()
{
}
