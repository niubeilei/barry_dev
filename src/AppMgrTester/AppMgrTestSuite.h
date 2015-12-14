////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgrTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgrTester_AppMgrTestSuite_h
#define Omn_AppMgrTester_AppMgrTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnAppMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnAppMgrTestSuite();
	~OmnAppMgrTestSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

