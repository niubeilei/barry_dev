////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxyTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_AppProxyTesterSuite_h
#define Omn_SslTester_AppProxyTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosAppProxyTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosAppProxyTestSuite() {}
	~AosAppProxyTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

