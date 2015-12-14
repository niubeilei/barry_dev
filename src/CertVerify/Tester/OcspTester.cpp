////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelUtil/KernelDebug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Porting/Sleep.h"

#include "CertVerify/Tester/OcspTester.h"
#include "CertVerify/Tester/OcspTestDriverUser.h"
#include "CertVerify/Tester/InitialOcspServer.h"
#include "KernelInterface/CliProc.h"

bool	AosOcspTester::start()
{
	OmnBeginTest <<"Test Ocsp functions";
    mTcNameRoot = "Test_Ocsp_functions";

	// 
	// Test default constructor
	//

	testOcsp_sendSerialnum();
	testOcsp_retriveResult();
	
	return true;
}


bool	AosOcspTester::testOcsp_sendSerialnum()
{
	int ret;
	
	//
	//	Initial Ocsp Server configuration
	//
	AosInitialOcspServer initialOcspServer(mTestMgr);
	initialOcspServer.start();
	
	//
	//	Test Case1: Start Ocsp Server
	//	 a. transfer a correct length serial number
	//	 	transfer a wrong length serial number
	//
	ret = AosOcspTester_Start_User(2, "A0200000000001", 14, 100, 1);
	OmnTC(OmnExpected<int>(true), OmnActual<int>(ret)) << endtc;
/*
	OmnSleep(100);	

	ret = AosOcspTester_Start_User(2, "A0200000000001", 14, 1000000, 10);
	OmnTC(OmnExpected<int>(true), OmnActual<int>(ret)) << endtc;
	
	OmnSleep(100);

	ret = AosOcspTester_Start_User(2, "A0200000000001", 14, 1000000, 100);
	OmnTC(OmnExpected<int>(true), OmnActual<int>(ret)) << endtc;

	OmnSleep(100);
*/	
	return true;
}

bool	AosOcspTester::testOcsp_retriveResult()
{
	int ret;
	//
	//	Test Case1: Retrieve result from Ocsp Server
	//
/*	u8 status;
	u32 valid, invalid, failed;
	ret = AosOcspTester_RetrieveRslt_User(2, &status, &valid, &invalid, &failed);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc; 
	OmnTC(OmnExpected<int>(9), OmnActual<int>(valid)) << endtc; 
	OmnTC(OmnExpected<int>(0), OmnActual<int>(invalid)) << endtc; 
	OmnTC(OmnExpected<int>(0), OmnActual<int>(failed)) << endtc; 
*/	
	return true;
}

