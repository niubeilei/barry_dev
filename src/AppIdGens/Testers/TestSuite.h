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
// 2013/01/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AppIdGens_Testers_TestSuite_h
#define Aos_AppIdGens_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosAppIdGensTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosAppIdGensTestSuite() {}
	~AosAppIdGensTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

