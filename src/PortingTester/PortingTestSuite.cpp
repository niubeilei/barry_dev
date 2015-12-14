////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PortingTestSuite.cpp
// Description:
//	This class defines the Util Library Test Suite. A Test Suite
//  consists of a number of Testers. This class will create
//  an instance of all the Util testers.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PortingTester/PortingTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "PortingTester/TimeTester.h"
#include "Util/OmnNew.h"


OmnPortingTestSuite::OmnPortingTestSuite()
{
}


OmnPortingTestSuite::~OmnPortingTestSuite()
{
}


OmnTestSuitePtr		
OmnPortingTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("PortingTestSuite", "Porting Library Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew OmnTimeTester());

	return suite;
}
