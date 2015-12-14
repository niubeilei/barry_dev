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
// 09/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_BitmapMgr_Tester_TestSuite_h
#define Omn_BitmapMgr_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosBitmapMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosBitmapMgrTestSuite();
	~AosBitmapMgrTestSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

