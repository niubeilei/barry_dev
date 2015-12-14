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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_XmlTorturer_TestSuite_h
#define Aos_SEUtil_XmlTorturer_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosSearchEngineTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSearchEngineTestSuite() {}
	~AosSearchEngineTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
};
#endif

