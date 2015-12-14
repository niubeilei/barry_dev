////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: QOSTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/QOSTorturer/QOSTestSuite.h"
#include "UnitTest/QOSTorturer/QOSTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

OmnQOSTestSuite::OmnQOSTestSuite()
{
}

OmnQOSTestSuite::~OmnQOSTestSuite()
{
}

OmnTestSuitePtr		
OmnQOSTestSuite::getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &serverIp,
							const OmnIpAddr &bouncerIp)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosQOSTester(localAddr,
												serverIp,
												bouncerIp));
	return suite;
}

