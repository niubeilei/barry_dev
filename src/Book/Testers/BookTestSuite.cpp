////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 12/15/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Book/Testers/BookTestSuite.h"

#include "Book/Testers/WebbookTester.h"
#include "Book/Testers/ContainerTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosBookTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Book", "Book Test Suite");

	// 
	// Now add all the testers
	//
	// suite->addTestPkg(OmnNew AosWebbookTester());
	suite->addTestPkg(OmnNew AosContainerTester());

	return suite;
}
