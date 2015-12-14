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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransClient_Tester_TestSuite_h
#define Aos_TransClient_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosTransClientTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosTransClientTestSuite() {}
	~AosTransClientTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
};
#endif

