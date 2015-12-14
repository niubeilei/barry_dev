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
// 05/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Testers_TestSuite_h
#define Aos_Actions_Testers_TestSuite_h

#include "Rundata/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosActionsTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosActionsTestSuite() {}
	~AosActionsTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
};
#endif

