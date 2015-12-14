////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemCliTesterSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef SystemCli_Tester_QuotaCliTesterSuite_h
#define SystemCli_Tester_QuotaCliTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class QuotaCliTesterSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	QuotaCliTesterSuite();
	~QuotaCliTesterSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

