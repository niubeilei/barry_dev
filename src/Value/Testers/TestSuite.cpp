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
// 2013/03/27 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Value/Testers/TestSuite.h"

#include "Value/Testers/ValueU64Tester.h"
#include "Value/Testers/ValueStrTester.h"
#include "Value/Testers/ValueCharStrTester.h"

#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosValueTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("Value", "Value Test Suite");

	// suite->addTestPkg(OmnNew AosValueStrTester());
	// suite->addTestPkg(OmnNew AosValueU64Tester());
	suite->addTestPkg(OmnNew AosValueCharStrTester());
	return suite;
}
