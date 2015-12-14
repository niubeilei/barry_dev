////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_SslTesterSuite_h
#define Omn_SslTester_SslTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosSslTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSslTestSuite() {}
	~AosSslTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

