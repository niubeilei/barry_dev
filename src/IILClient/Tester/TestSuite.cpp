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
#include "IILClient/Tester/TestSuite.h"

#include "IILClient/Tester/IILHitTester.h"
#include "IILClient/Tester/IILStrTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosIILClientTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("IILClientTestSuite", "IILClient Test Suite");

	//suite->addTestPkg(OmnNew AosIILHitTester());
	suite->addTestPkg(OmnNew AosIILStrTester());

	return suite;
}

