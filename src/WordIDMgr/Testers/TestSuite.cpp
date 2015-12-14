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
// 2015/01/30 Created  
////////////////////////////////////////////////////////////////////////////
#include "WordIDMgr/Testers/TestSuite.h"

#include "WordIDMgr/Testers/WordIDMgrTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosWordIDMgrTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("WordIDMgrTorturer", "WordIDMgrTorturer Test Suite");

	// suite->addTestPkg(OmnNew AosBitmapExecutorTester());
	// suite->addTestPkg(OmnNew AosBitmapCacheTester());
	// suite->addTestPkg(OmnNew AosIndexMgrTester());
//	suite->addTestPkg(OmnNew AosSmartQueryTester());
	suite->addTestPkg(OmnNew AosWordIDMgrTester());

	return suite;
}

