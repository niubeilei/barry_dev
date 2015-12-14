////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LicenseManagementTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliTester/LicenseManagementTester.h"
#include "InitialEnvironment.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool AosLicenseManagementTester::start()
{
	// 
	// Test default constructor
	//
	testShowLicense();
	testTurnFeature();
	testVerifyLicense();
	testImportLicense();

	integrateTest();
	capacityTest();

	return true;
}


	// 
	// This test file includes Test cases at least as below: 
	// 		Test Case 1:	Show license information	
	// 		Test Case 2:	Turn a feature on/off
	// 		Test Case 3:	Verify license information
	//		Test Case 4:	Import a License
	//		Test Case 20:	License integrate Test
	//		Test Case 21:	License capacity Test
	//  


bool AosLicenseManagementTester::testShowLicense()
{

	OmnBeginTest <<"Test Show license CLI ";
	mTcNameRoot = "Test_CLI_Show_license";

	AosInitialEnvironment initialEnvironment(mTestMgr);
	initialEnvironment.start();

	// 
	// Test Case 1: Show license information
	//	


	return true;	
}


bool AosLicenseManagementTester::testTurnFeature()
{

    OmnBeginTest <<"Test Turn feature on/off CLI ";
    mTcNameRoot = "Test_CLI_Turn_Feature";

    AosInitialEnvironment initialEnvironment(mTestMgr);
    initialEnvironment.start();

	// 
	// Test Case 2: Turn on/off a feature
	//	

	

	return true;	
}


bool AosLicenseManagementTester::testVerifyLicense()
{

    OmnBeginTest <<"Test verify license CLI ";
    mTcNameRoot = "Test_CLI_Verifiy_license";

    AosInitialEnvironment initialEnvironment(mTestMgr);
    initialEnvironment.start();

	// 
	// Test Case 3: Verify license information
	//	
	
	

	return true;	
}


bool AosLicenseManagementTester::testImportLicense()
{

    OmnBeginTest <<"Test import license CLI ";
    mTcNameRoot = "Test_CLI_Import_license";

    AosInitialEnvironment initialEnvironment(mTestMgr);
    initialEnvironment.start();

	// 
	// Test Case 4: Import a license
	//	

	return true;	
}


bool AosLicenseManagementTester::integrateTest()
{

    OmnBeginTest <<"Test license integrate CLI ";
    mTcNameRoot = "Test_CLI_Integrate_license";

    AosInitialEnvironment initialEnvironment(mTestMgr);
    initialEnvironment.start();

	// 
	// Test Case 20: License integrate test
	//	

	return true;	
}


bool AosLicenseManagementTester::capacityTest()
{

    OmnBeginTest <<"Test license capacity CLI ";
    mTcNameRoot = "Test_CLI_Capacity_license";

    AosInitialEnvironment initialEnvironment(mTestMgr);
    initialEnvironment.start();

	// 
	// Test Case 21: license capacity test
	//	

	return true;	
}

