////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliQos_Tester_AosCliQosTester_h
#define Aos_CliQos_Tester_AosCliQosTester_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosCliQosTestSuite
{
private:

public:
	AosCliQosTestSuite() {}
	~AosCliQosTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

