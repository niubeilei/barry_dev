////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FWTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/FWTorturer/FWTestSuite.h"
#include "UnitTest/FWTorturer/FWTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnFWTestSuite::OmnFWTestSuite()
{
}

OmnFWTestSuite::~OmnFWTestSuite()
{
}

OmnTestSuitePtr		
OmnFWTestSuite::getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &serverIp,
							const OmnIpAddr &bouncerIp)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosFWTester(localAddr,
												serverIp,
												bouncerIp));
	return suite;
}

