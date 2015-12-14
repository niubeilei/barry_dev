////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.h
// Description:
//   
//
// Modification History:
// 01/12/2010 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_TestSuite_h
#define Aos_SEClient_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosSEClientTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSEClientTestSuite() {}
	~AosSEClientTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

