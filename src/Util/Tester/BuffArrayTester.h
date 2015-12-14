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
// 2013/02/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_BuffArrayTester_h
#define Omn_TestUtil_BuffArrayTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class AosBuffArrayTester : public OmnTestPkg
{
private:
	int		mTries;

public:
	AosBuffArrayTester();
	~AosBuffArrayTester() {}
	AosCompareFunPtr    mCmpFun;

	virtual bool		start();

private:
	bool		testInsert();
};
#endif

