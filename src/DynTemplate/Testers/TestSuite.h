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
// 2013/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DynTemplate_Testers_TestSuite_h
#define Omn_DynTemplate_Testers_TestSuite_h

#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDynTemplateTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDynTemplateTestSuite() {}
	~AosDynTemplateTestSuite() {}

	static OmnTestSuitePtr		getSuite(const OmnString &type);
};
#endif

