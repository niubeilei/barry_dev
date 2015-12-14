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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
//#ifndef Omn_IILTrans_Testers_TestSuite_h
//#define Omn_IILTrans_Testers_TestSuite_h
#ifndef Aos_Util_Testers2_TestSuite_h
#define Aos_Util_Testers2_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosUtilTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosUtilTestSuite() {}
	~AosUtilTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif
