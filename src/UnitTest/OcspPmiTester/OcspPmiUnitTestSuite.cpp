////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspPmiUnitTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/OcspPmiTester/OcspPmiUnitTestSuite.h"
#include "UnitTest/OcspPmiTester/OcspPmiTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnOcspPmiUnitTestSuite::OmnOcspPmiUnitTestSuite()
{
}

OmnOcspPmiUnitTestSuite::~OmnOcspPmiUnitTestSuite()
{
}

OmnTestSuitePtr		
OmnOcspPmiUnitTestSuite::getSuite(const OmnIpAddr &clientIp, 
							const OmnIpAddr &serverIp,
							const OmnIpAddr &bouncerIp,
							const OmnIpAddr &PMIServerIp)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosOcspPmiTester(clientIp, 
												serverIp,
												bouncerIp,
												PMIServerIp));
	return suite;
}

