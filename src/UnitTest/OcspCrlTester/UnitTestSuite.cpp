////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UnitTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/OcspCrlTester/UnitTestSuite.h"
#include "UnitTest/OcspCrlTester/OcspCrlTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnUnitTestSuite::OmnUnitTestSuite()
{
}

OmnUnitTestSuite::~OmnUnitTestSuite()
{
}

OmnTestSuitePtr		
OmnUnitTestSuite::getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &clientIp1,
							const OmnIpAddr &clientIp2, 
							const OmnIpAddr &serverIp,
							const OmnIpAddr &bouncerIp,
							const OmnIpAddr &OcspCrlServerIp)
{
	OmnTestSuitePtr suite = OmnNew	OmnTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
//	suite->addTestPkg(OmnNew AosOcspCrlTester());
	suite->addTestPkg(OmnNew AosOcspCrlTester(localAddr,
												clientIp1,
												clientIp2,
												serverIp,
												bouncerIp,
												OcspCrlServerIp));
	return suite;

}

