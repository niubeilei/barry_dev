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
// 07/26/2010 	Created by James Kong
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_Testers_TestSuite_h
#define Aos_SEUtil_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosSEUtilTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSEUtilTestSuite() {}
	~AosSEUtilTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

