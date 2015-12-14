////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SinglePmiUnitTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/PmiTester/SinglePmiUnitTestSuite.h"
#include "UnitTest/PmiTester/SinglePmiTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnSinglePmiUnitTestSuite::OmnSinglePmiUnitTestSuite()
{
}

OmnSinglePmiUnitTestSuite::~OmnSinglePmiUnitTestSuite()
{
}

OmnTestSuitePtr		
OmnSinglePmiUnitTestSuite::getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &clientIp1,
							const OmnIpAddr &clientIp2,
							const OmnIpAddr &clientIp3, 
							const OmnIpAddr &serverIp,
							const OmnIpAddr &bouncerIp,
							const OmnIpAddr &PMIServerIp)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosSinglePmiTester(localAddr,
												clientIp1,
												clientIp2,
												clientIp3, 
												serverIp,
												bouncerIp,
												PMIServerIp));
	return suite;
}

