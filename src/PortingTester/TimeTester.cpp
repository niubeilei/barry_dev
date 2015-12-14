////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimeTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PortingTester/TimeTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Porting/TimeOfDay.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

OmnTimeTester::OmnTimeTester()
{
	mName = "OmnTimeTester";
}


OmnTimeTester::~OmnTimeTester()
{
}


bool OmnTimeTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "???-DFT-CTR";

	int counts[50];
	int i;
	for (i=0; i<50; i++)
	{
		counts[i] = GetTickCount();
	}

	for (i=0; i<50; i++)
	{
		cout << "Count: " << counts[i] << endl;
	}

	LARGE_INTEGER t, t2;
	if (QueryPerformanceFrequency(&t2))
	{
		cout << "Found: " << (int)t2.QuadPart << endl;
	}
	else
	{
		cout << "Failed" << endl;
	}

	if (QueryPerformanceCounter(&t))
	{
		cout << "Begin time: " << (int)t.QuadPart << endl;

//		for (int i=0; i<10000; i++)
//		{
//			GetTickCount();
//		}
	
		LARGE_INTEGER t1;
		QueryPerformanceCounter(&t1);
		cout << "End time: " << (int)t1.QuadPart << endl;
		cout << "Diff: " << (int)(t1.QuadPart - t.QuadPart) << endl;
		cout << "Time in msec: " << (float)(t1.QuadPart - t.QuadPart) / (float)t2.QuadPart * 1000 << endl;
	}
	else
	{
		cout << "Error" << endl;
	}
	return true;
}

