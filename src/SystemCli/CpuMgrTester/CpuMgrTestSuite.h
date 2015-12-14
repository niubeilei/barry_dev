////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CpuMgrTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SystemCli_CpuMgrTester_CpuMgrTestSuite_h
#define Omn_SystemCli_CpuMgrTester_CpuMgrTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosCpuMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosCpuMgrTestSuite();
	virtual ~AosCpuMgrTestSuite(); 

	static OmnTestSuitePtr		getSuite();
};
#endif

