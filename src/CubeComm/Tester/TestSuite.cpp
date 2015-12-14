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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/Tester/TestSuite.h"
#include "CubeComm/Tester/CubeCommTester.h"
//#include "CubeComm/Tester/DataCommTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosCubeCommTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("CubeCommTestSuite", "CubeComm Test Suite");

	// Now add all the testers
	//suite->addTestPkg(OmnNew AosDataCommTester());
	suite->addTestPkg(OmnNew AosCubeCommTester());

	return suite;
}
