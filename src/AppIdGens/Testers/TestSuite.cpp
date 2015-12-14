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
// 2013/01/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AppIdGens/Testers/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"

#include "AppIdGens/Testers/AppIdGenU64Tester.h"


OmnTestSuitePtr		
AosAppIdGensTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("AppIdGens", "AppIdGens");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosAppIdGenU64Tester());

	return suite;
}
