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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_Tester_DataCommTester_h
#define Aos_CubeComm_Tester_DataCommTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class AosDataCommTester : public OmnTestPkg
{
private:
	int		mTries;
	int 	mBatchSize;
	int		mTestDurationSec;

public:
	AosDataCommTester();
	~AosDataCommTester() {}

	virtual bool		start();
	bool basicTest();
	bool runOneTest(const int batch_size);

private:
};
#endif

