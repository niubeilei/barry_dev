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
// 2012/11/13 Created by Ken Lee
// ////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_Testers_TestSuite_h
#define Aos_DataJoin_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDataJoinTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDataJoinTestSuite();
	~AosDataJoinTestSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

