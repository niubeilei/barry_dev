////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMgrTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SystemCli_MemMgrTester_MemMgrTestSuite_h
#define Omn_SystemCli_MemMgrTester_MemMgrTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosMemMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosMemMgrTestSuite();
	virtual ~AosMemMgrTestSuite(); 

	static OmnTestSuitePtr		getSuite();
};
#endif

