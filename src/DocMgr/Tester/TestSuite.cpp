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
// 2013/05/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocMgr/Tester/TestSuite.h"

#include "DocMgr/Tester/DocMgrTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosDocMgrTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TestSuite", "Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosDocMgrTester());
	return suite;
}
