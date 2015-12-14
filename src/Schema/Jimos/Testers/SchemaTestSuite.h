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
// 2013/10/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Schema_Jimos_Testers_SchemaTestSuite_h
#define Omn_Schema_Jimos_Testers_SchemaTestSuite_h

#include "Tester/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosSchemaTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSchemaTestSuite() {}
	~AosSchemaTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

