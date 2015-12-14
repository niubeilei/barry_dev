////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelUtilTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliTester/CliTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "KernelUtilTester/CharPtreeTester.h"
#include "KernelUtilTester/HashIntTester.h"

OmnTestSuitePtr		
OmnCliTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("CoreTestSuite", "aos_core Library Test Suite");
    
	// 
	// Now add all the testers
	//
	//suite->addTestPkg(OmnNew AosCharPtreeTester());
    suite->addTestPkg(OmnNew AosHashIntTester());
	return suite;
}

