////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PortingTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PortingTester_PortingTestSuite_h
#define Omn_PortingTester_PortingTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnPortingTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnPortingTestSuite();
	~OmnPortingTestSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

