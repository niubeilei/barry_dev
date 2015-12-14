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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SuperTable_SuperTableTestSuite_h
#define Omn_SuperTable_SuperTableTestSuite_h

#include "Tester/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosSuperTableTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSuperTableTestSuite() {}
	~AosSuperTableTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

