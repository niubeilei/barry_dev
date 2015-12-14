////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemCliTesterSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SystemCli/QuotaTester/QuotaCliTesterSuite.h"
#include "SystemCli/QuotaTester/QuotaCliTester.h"

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


OmnTestSuitePtr		
QuotaCliTesterSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("QuotaCLiTestSuite", "SystemCli Test Suite");


	// 
	// Now add all the testers
	//
    suite->addTestPkg(OmnNew QuotaCliTester());
 
	return suite;
}


QuotaCliTesterSuite::QuotaCliTesterSuite()
{
}


QuotaCliTesterSuite::~QuotaCliTesterSuite()
{
}