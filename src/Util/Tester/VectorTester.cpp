////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
//	12/17/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/VectorTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Vector.h"
#include "Util/OmnNew.h"



bool AosVectorTester::start()
{
	torturer();
	return true;
}


bool
AosVectorTester::torturer()
{
	//const int lsMaxValues = 10000;
	//OmnVector<int, 10, 10, lsMaxValues> qq;
	// aos_vector<u64> testVector;
	vector<u64> testVector;

	int tries = 0;//mTestMgr->getTries();
	if (tries <= 0) tries = 500000;
	u64 statrTime = OmnGetTimestamp();

	/*
	cout << "Testing aos_vector..." << endl;
	for(size_t i = 0 ; i < tries; i++) 
	{
		// if (i % 10000 == 0) cout << "i = " << i << endl;
		aos_vector<u64> vv;
		for (u32 k=0; k<1000; k++) vv.push_back(i);
		for (u32 k=0; k<vv.size(); k++) vv[k] = k;
	}

	u64 endTime1 = OmnGetTimestamp();
	cout << "Testing vector..." << endl;
	for(size_t i = 0 ; i < tries; i++) 
	{
		// if (i % 10000 == 0) cout << "i = " << i << endl;
		vector<u64> vv;
		for (u32 k=0; k<1000; k++) vv.push_back(i);
		for (u32 k=0; k<vv.size(); k++) vv[k] = k;
	}
	*/

	cout << "Testing aos_vector..." << endl;
	for(size_t i = 0 ; i < tries; i++) 
	{
		// if (i % 10000 == 0) cout << "i = " << i << endl;
		aos_vector<OmnString> vv;
		for (u32 k=0; k<100; k++) vv.push_back("12345");
		for (u32 k=0; k<vv.size(); k++) vv[k] = "45678";
	}

	u64 endTime1 = OmnGetTimestamp();
	cout << "Testing vector..." << endl;
	for(size_t i = 0 ; i < tries; i++) 
	{
		// if (i % 10000 == 0) cout << "i = " << i << endl;
		vector<OmnString> vv;
		for (u32 k=0; k<100; k++) vv.push_back("12345");
		for (u32 k=0; k<vv.size(); k++) vv[k] = "45678";
	}
	u64 endTime2 = OmnGetTimestamp();
	cout << "The result: " << (endTime1 - statrTime) << endl;
	cout << "The result: " << (endTime2 - endTime1) << endl;
	
	return true;
}

