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
#include "SuperTable/Testers/SuperTableTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosSuperTableTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SuperTable", "SuperTable Test Suite");

//	suite->addTestPkg(OmnNew AosUtUtilTester());

	return suite;
}
