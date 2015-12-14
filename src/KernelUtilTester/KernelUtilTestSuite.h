////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelUtilTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_KernelUtilTest_h
#define Omn_KernelUtil_KernelUtilTest_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnKernelUtilTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnKernelUtilTestSuite() {}
	~OmnKernelUtilTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

