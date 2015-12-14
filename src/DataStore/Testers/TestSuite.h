////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 12/15/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DataStore_Testers_TestSuite_h
#define Omn_DataStore_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDataStoreTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDataStoreTestSuite() {}
	~AosDataStoreTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

