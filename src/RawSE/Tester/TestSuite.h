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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSE_Tester_TestSuite_h
#define Aos_RawSE_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Tester/TestPkg.h"
#include "RawSE/RawSE.h"
class AosRawSETestSuite:public OmnTestPkg
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosRawSETestSuite() {}
	~AosRawSETestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

