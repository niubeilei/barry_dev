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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Scheduler/Testers/SchedulerTestSuite.h"
#include "Scheduler/Testers/SchedulerTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosSchedulerTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Scheduler", "Scheduler Test Suite");

	suite->addTestPkg(OmnNew AosSchedulerTester());

	return suite;
}
