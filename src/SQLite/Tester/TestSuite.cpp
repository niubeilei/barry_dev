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
// 04/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SQLite/Tester/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"

#include "SQLite/Tester/DbTester.h"


OmnTestSuitePtr		
OmnSQLiteTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SQLiteTestSuite", "SQLite Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew OmnDbTester());

	return suite;
}
