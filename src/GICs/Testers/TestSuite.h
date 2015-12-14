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
// 08/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_TesterNew_TestSuite_h
#define Aos_GICs_TesterNew_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosGicsTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosGicsTestSuite() {}
	~AosGicsTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
};
#endif
