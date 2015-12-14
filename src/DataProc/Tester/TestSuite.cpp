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
// 2013/06/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/Tester/TestSuite.h"

#include "DataProc/Tester/DataProcComposeSingleTester.h"
#include "DataProc/Tester/DataProcWordParserTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosDataProcComposeSingleTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TestSuite", "Test Suite");

	// Now add all the testers
	//suite->addTestPkg(OmnNew AosDataProcComposeSingleTester());
	suite->addTestPkg(OmnNew AosDataProcWordParserTester());
	return suite;
}
