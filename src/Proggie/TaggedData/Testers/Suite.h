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
// 08/01/2009: 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Proggie_TaggedData_Testers_Suite_h
#define Aos_Proggie_TaggedData_Testers_Suite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosTaggedDataTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosTaggedDataTestSuite() {}
	~AosTaggedDataTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

