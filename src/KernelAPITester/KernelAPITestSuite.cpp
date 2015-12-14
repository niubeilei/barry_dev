////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelAPITestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "kernelAPITester/KernelAPITestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "KernelAPITester/AppManagementTester.h"
#include "Util/OmnNew.h"


OmnKernelAPITestSuite::OmnKernelAPITestSuite()
{
}


OmnKernelAPITestSuite::~OmnKernelAPITestSuite()
{
}


OmnTestSuitePtr		
OmnKernelAPITestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("KernelAPITestSuite", "Kernel API Test Suite");
	suite.setDelFlag(false);

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew OmnAppManagementTester());

	return suite;
}
