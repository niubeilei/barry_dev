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
// 2011/07/11	Created by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#include "MultiCondQuery/TestSuite.h"

#include "MultiCondQuery/SmartQueryTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosSmartQueryTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SmartQueryTestSuite", "SmartQuery Test Suite");

	suite->addTestPkg(OmnNew AosSmartQueryTester());

	return suite;
}

