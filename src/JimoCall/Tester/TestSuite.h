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
// 2014/12/05 Created by Chen Dign
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_Tester_TestSuite_h
#define Aos_JimoCall_Tester_TestSuite_h

#include "JimoCall/Tester/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosJimoCallTestSuite
{
	OmnDefineRCObject;

private:
	AosJimoCallTestSuitePtr		mSuite;

public:
	AosJimoCallTestSuite() {}
	~AosJimoCallTestSuite() {}

	static OmnTestSuitePtr getSuite();
};
#endif

