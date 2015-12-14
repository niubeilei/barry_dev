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
// 2011/07/11	created by: Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MultiCondQuery_TestSuite_h
#define Aos_MultiCondQuery_TestSuite_h 

#include "Tester/TestSuite.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"

class AosSmartQueryTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSmartQueryTestSuite() {}
	~AosSmartQueryTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
};

#endif

