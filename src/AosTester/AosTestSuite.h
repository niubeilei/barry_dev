////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosTester_AosTestSuite_h
#define Omn_AosTester_AosTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosTestSuite() {}
	~AosTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

