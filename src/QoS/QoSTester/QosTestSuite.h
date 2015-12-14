////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: QoSTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_QoSTester_QoSTesterSuite_h
#define Omn_QoSTester_QoSTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosQoSTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosQoSTestSuite() {}
	~AosQoSTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

