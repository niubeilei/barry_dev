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
// 2014/08/10 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_VarUnintTester_h
#define Omn_TestUtil_VarUnintTester_h

#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"


class AosVarUnintTester : public OmnTestPkg
{
private:
	enum
	{
		 eBuffLen = 1000*1000*1000
	};                                  

	char*			mData;
	i64				mIdx;

	vector <u64> 	mValues;
	vector <char>	mDocidFlags;
	AosRundataPtr	mRundata;
	AosBuffPtr		mBuff;
	AosBuff *		mBuffRaw;

public:
	AosVarUnintTester();
	~AosVarUnintTester() {}
	virtual bool		start();


private:
	bool	basicTest();
	bool	basicTest2();
	bool 	performanceTest();
	bool	addValues();
	bool 	verify();
	bool	addValuesToBuff();
	bool 	verifyFromBuff();
};

#endif

