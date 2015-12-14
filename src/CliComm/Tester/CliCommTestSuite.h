////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliComm_Tester_CliCommTestSuite_h
#define Omn_CliComm_Tester_CliCommTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosCliCommTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosCliCommTestSuite() {}
	~AosCliCommTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

