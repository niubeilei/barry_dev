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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester2/UtilTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/Tester2/ValueRsltTester.h"
#include "Util/Tester2/ValueRsltPerformances.h"

OmnTestSuitePtr		
AosUtilTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TestSuite", "IILTrans Library Test Suite");
	//suite->addTestPkg(OmnNew AosValueRsltTester());
	suite->addTestPkg(OmnNew AosValueRsltPerformances());
	return suite;
}
