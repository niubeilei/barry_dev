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
#include "IILClient/Tester2/TestSuite.h"

#include "IILClient/Tester2/IILI64D64Tester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosIILClientTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("IILClientTestSuite2", "IILClient Test Suite");

	suite->addTestPkg(OmnNew AosIILI64D64Tester());

	return suite;
}

