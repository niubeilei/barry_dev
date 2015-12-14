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
// 2013/05/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_Tester_TestSuite_h
#define Omn_UtilTime_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosUtilTimeTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosUtilTimeTestSuite() {}
	~AosUtilTimeTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

