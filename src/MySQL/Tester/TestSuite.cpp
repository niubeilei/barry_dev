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
// 05/01/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MySQL/Tester/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "MySQL/Tester/DbTester.h"


OmnTestSuitePtr		
AosMySQLTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("MySQLTestSuite", "MySQL Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew OmnDbTester());

	return suite;
}
