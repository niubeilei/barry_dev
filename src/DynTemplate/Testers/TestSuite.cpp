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
// 2013/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynTemplate/Testers/TestSuite.h"

#include "ActionDyn/Testers/DLLTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosDynTemplateTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DynTemplate", "DynTemplate Test Suite");

	// suite->addTestPkg(OmnNew AosSampleTester());
	return suite;
}
