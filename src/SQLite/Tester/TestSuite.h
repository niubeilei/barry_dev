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
// 04/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SQLite_Tester_TestSuite_h
#define Aos_SQLite_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnSQLiteTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnSQLiteTestSuite() {}
	~OmnSQLiteTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

