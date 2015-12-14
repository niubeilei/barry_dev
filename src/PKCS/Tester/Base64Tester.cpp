////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Base64Tester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/Tester/Base64Tester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"



bool AosBase64Tester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosBase64Tester::basicTest()
{
	OmnBeginTest << "Test SSL Base64";
	mTcNameRoot = "Test_SSLBase64";

	// OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
			
	return true;
}	

