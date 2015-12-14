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
// 2013/03/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Value_Testers_TestSuite_h
#define Aos_Value_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosValueTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosValueTestSuite() {}
	~AosValueTestSuite() {}

	static OmnTestSuitePtr		getSuite(const OmnString &type);
};
#endif

