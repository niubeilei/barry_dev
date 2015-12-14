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
// 02/26/2012 Created By Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/TransMgr/Tester/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "SmartObj/TransMgr/Tester/SobjTransMgrTester.h"


OmnTestSuitePtr		
SobjTransMgrTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SobjTransMgr", "Test Suite");

	suite->addTestPkg(OmnNew SobjTransMgrTester());
	return suite;
}

