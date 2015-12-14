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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoExample/Testers/TestSuite.h"

#include "ActionDyn/Testers/DLLTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosJimoExampleTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("JimoExample", "JimoExample Test Suite");

	// suite->addTestPkg(OmnNew AosSampleTester());
	return suite;
}
