////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AosTester/AosTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr
AosTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("AosTestSuite", "Aos Test Suite");

	// 
	// Now add all the testers
	//
	// suite->addTestPkg(OmnNew OmnAosKernelApiTester());

	return suite;
}

