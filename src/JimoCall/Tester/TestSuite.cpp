////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.cpp
// Description:
//	This class defines the Util Library Test Suite. A Test Suite
//  consists of a number of Testers. This class will create
//  an instance of all the Util testers.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/Tester/TestSuite.h"

#include "JimoCall/Tester/JimoCallTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr
AosJimoCallTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("JimoCall", "JimoCall");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosJimoCallTester());

	return suite;
}
