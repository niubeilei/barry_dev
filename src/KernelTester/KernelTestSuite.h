////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_KernelTest_h
#define Omn_KernelUtil_KernelTest_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnKernelTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnKernelTestSuite() {}
	~OmnKernelTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

