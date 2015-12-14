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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/Tester/TestSuite.h"

#include "JimoAPI/Tester/JimoAPITester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosJimoAPITestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("JimoAPITestSuite", "JimoAPI Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosJimoAPITester());

	return suite;
}
