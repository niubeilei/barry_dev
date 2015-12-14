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
// 2011/06/15	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterClt_Tester_TestSuite_h
#define Aos_CounterClt_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "SEUtil/SeXmlParser.h"


class AosCounterCltTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosCounterCltTestSuite() {}
	~AosCounterCltTestSuite() {}

	static OmnTestSuitePtr getSuite(const AosXmlTagPtr &testers);
};
#endif

