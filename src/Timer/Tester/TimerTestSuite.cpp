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
// 2011/6/17	Created by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#include "Timer/Tester/TimerTestSuite.h"

#include "Timer/Tester/TimerTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosTimerTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UtilTestSuite", "Util Library Test Suite");

	suite->addTestPkg(OmnNew AosTimerTester());

	return suite;
}
