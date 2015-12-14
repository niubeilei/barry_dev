////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.h
// Description:
//   
//
// Modification History:
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocSelector_Testers_TestSuite_h
#define Aos_DocSelector_Testers_TestSuite_h

#include "SEUtil/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDocSelectorTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDocSelectorTestSuite() {}
	~AosDocSelectorTestSuite() {}

	static OmnTestSuitePtr getSuite(const AosXmlTagPtr &testers);
};
#endif

