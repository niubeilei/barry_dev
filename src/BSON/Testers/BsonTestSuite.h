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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_BSON_Testers_BsonTestSuite_h
#define Omn_BSON_Testers_BsonTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosBsonTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosBsonTestSuite() {}
	~AosBsonTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

