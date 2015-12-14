////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BridgeTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SystemCli_BridgeTester_BridgeTesterSuite_h
#define Omn_SystemCli_BridgeTester_BridgeTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosBridgeTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosBridgeTestSuite();
	virtual ~AosBridgeTestSuite(); 

	static OmnTestSuitePtr		getSuite();
};
#endif

