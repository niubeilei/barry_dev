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
// 2015/04/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_IDOTester_IDOTestSuite_h
#define Omn_IDOTester_IDOTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosIDOTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosIDOTestSuite() {}
	~AosIDOTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

