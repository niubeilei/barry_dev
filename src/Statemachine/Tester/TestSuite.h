////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StmcTestSuite.h
// Description:
//   
//
// Modification History:
// 01/06/2012	Created By Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_StmcTestSuite_h
#define Omn_TestUtil_StmcTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosStmcTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosStmcTestSuite() {}
	~AosStmcTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

