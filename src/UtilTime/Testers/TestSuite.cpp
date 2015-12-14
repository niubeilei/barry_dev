////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2013/05/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/Testers/TestSuite.h"

#include "UtilTime/Testers/TFEpochDayTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosUtilTimeTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TestSuite", "Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosTFEpochDayTester());
	return suite;
}
