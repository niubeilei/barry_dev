////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WebWallTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/WebWallTorturer/WebWallTestSuite.h"
#include "UnitTest/WebWallTorturer/WebWallTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnWebWallTestSuite::OmnWebWallTestSuite()
{
}

OmnWebWallTestSuite::~OmnWebWallTestSuite()
{
}

OmnTestSuitePtr		
OmnWebWallTestSuite::getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &serverIp,
							const OmnIpAddr &bouncerIp)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosWebWallTester(localAddr,
												serverIp,
												bouncerIp));
	return suite;
}

