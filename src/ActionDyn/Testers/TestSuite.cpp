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
// 2013/05/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ActionDyn/Testers/TestSuite.h"

#include "ActionDyn/Testers/SampleTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosActionDynTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("ActionDyn", "ActionDyn Test Suite");

	if (type == "sample_tester")
	{
		suite->addTestPkg(OmnNew AosSampleTester());
	}
	else if(type == "help" || type == "--help")
	{
		cout << "Testers can be one of the following:\n"
			 << "count_test \n"
			 << "all        \n";
		exit(0);
	}
	else
	{
		suite->addTestPkg(OmnNew AosSampleTester());
	}
	return suite;
}
