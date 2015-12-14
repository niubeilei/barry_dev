////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: coreTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_coreTester_coreTestSuite_h
#define Omn_coreTester_coreTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnCoreTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnCoreTestSuite() {}
	~OmnCoreTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

