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
// 08/09/2013 	Created by Young pan 
///////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILMgr_Tester_TestSuite_h
#define Aos_IILMgr_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Tester/TestPkg.h"


class AosIILTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosIILTestSuite() {}
	~AosIILTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

