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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Scheduler_SchedulerTestSuite_h
#define Omn_Scheduler_SchedulerTestSuite_h

#include "Tester/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosSchedulerTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSchedulerTestSuite() {}
	~AosSchedulerTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

