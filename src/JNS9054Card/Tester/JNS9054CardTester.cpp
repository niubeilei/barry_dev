////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTester.cpp
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

#include "KernelInterface/CliProc.h"
#include "JNS9054Card/Tester/JNS9054CardTester.h"

bool	AosJNS9054CardTester::start()
{
	OmnBeginTest <<"Test JNS9054Card API";
    mTcNameRoot = "Test_JNS9054Card_API";

	// 
	// Test default constructor
	//
	testJNS9054Card_Start();
	testJNS9054Card_Retrive();
	
	return true;
}

bool	AosJNS9054CardTester::testJNS9054Card_Start()
{


	return true;
}

bool	AosJNS9054CardTester::testJNS9054Card_Retrive()
{


	return true;
}
