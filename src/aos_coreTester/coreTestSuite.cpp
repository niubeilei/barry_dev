////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: coreTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_coreTester/coreTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "aos_coreTester/aosBridgeTester.h"
#include "aos_coreTester/aosKernelApiTester.h"
#include "aos_coreTester/inputSkbTester.h"


OmnTestSuitePtr		
OmnCoreTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("CoreTestSuite", "aos_core Library Test Suite");

	// 
	// Now add all the testers
	//
	// suite->addTestPkg(OmnNew OmnAosKernelApiTester());
//	suite->addTestPkg(OmnNew OmnAosBridgeTester());
	suite->addTestPkg(OmnNew OmnInputSkbTester());

	return suite;
}

