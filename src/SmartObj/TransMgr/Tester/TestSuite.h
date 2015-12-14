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
// 02/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_TransMgr_Tester_TestSuite_h
#define Aos_SmartObj_TransMgr_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class SobjTransMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	SobjTransMgrTestSuite() {}
	~SobjTransMgrTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

