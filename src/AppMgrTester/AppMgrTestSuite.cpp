////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgrTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgrTester/AppMgrTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "AppMgrTester/AppMgrTester.h"
#include "Util/OmnNew.h"


OmnAppMgrTestSuite::OmnAppMgrTestSuite()
{
}


OmnAppMgrTestSuite::~OmnAppMgrTestSuite()
{
}


OmnTestSuitePtr		
OmnAppMgrTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("AppMgrTestSuite", "AppMgr Test Suite");
	suite.setDelFlag(false);

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew OmnAppMgrTester());

	return suite;
}
