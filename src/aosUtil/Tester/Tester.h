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

#ifndef Omn_aosUtil_Tester_aosUtilTestSuite_h
#define Omn_aosUtil_Tester_aosUtilTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnAosUtilTestSuite
{
private:

public:
	OmnAosUtilTestSuite() {}
	~OmnAosUtilTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

