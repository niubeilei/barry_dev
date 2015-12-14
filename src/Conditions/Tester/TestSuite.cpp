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
// 01/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conditions/Tester/TestSuite.h"

#include "Conditions/Tester/CondTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosCondTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Conditions", "Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosCondTester());

	return suite;
}

