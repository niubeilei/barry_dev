////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosUtilTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtilTester/aosUtilTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "aosUtilTester/skbGeneratorTester.h"


OmnTestSuitePtr		
OmnAosUtilTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("aosUtilTestSuite", "aosUtil Library Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosSkbGeneratorTester());

	return suite;
}

