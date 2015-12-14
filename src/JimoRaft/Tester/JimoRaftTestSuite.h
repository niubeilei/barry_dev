////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2015-1-8 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoRaft_Tester_TestSuite_h
#define Aos_JimoRaft_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Tester/TestPkg.h"
class AosJimoRaftTestSuite:public OmnTestPkg
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosJimoRaftTestSuite() {}
	~AosJimoRaftTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

