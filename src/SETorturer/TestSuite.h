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
// 08/28/2010 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SETorturer_TestSuite_h
#define Aos_SETorturer_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosSETorturerTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSETorturerTestSuite() {}
	~AosSETorturerTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

