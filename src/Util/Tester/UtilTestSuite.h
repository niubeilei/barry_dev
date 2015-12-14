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
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_UtilTestSuite_h
#define Omn_TestUtil_UtilTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnUtilTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnUtilTestSuite() {}
	~OmnUtilTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

