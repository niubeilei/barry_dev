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
// 2015/04/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SysIDOs/Tester/IDOTestSuite.h"

#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "SysIDOs/Tester/IDOTester.h"


OmnTestSuitePtr		
AosIDOTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("IDOTestSuite", "SysIDOs Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosIDOTester());

	return suite;
}
