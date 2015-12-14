////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PkcsTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_PkcsTester_PkcsTesterSuite_h
#define Aos_PkcsTester_PkcsTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosPkcsTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosPkcsTestSuite() {}
	~AosPkcsTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

