////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TracerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/TracerTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/Tracer.h"



bool AosTracerTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosTracerTester::basicTest()
{
	OmnBeginTest << "Test AosTracer";
	mTcNameRoot = "TracerTest";

	char buff[99+1];
	for (int i=0; i<32; i++)
	{
		buff[i] = i;
	}

	aos_trace("+++++++++++++To dump\n");
	aos_trace_hex("Test", buff, 32);
	aos_trace("+++++++++++++Finished dump\n");

	// OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}



