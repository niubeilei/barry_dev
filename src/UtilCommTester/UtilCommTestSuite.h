////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilCommTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilcommTester_UtilCommTestSuite_h
#define Omn_UtilcommTester_UtilCommTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnUtilCommTestSuite
{
private:
	OmnTestSuitePtr		mSuite;
	static bool			mIsServer;

public:
	OmnUtilCommTestSuite();
	~OmnUtilCommTestSuite() {}

	static OmnTestSuitePtr		getSuite(const int argc, char **argv);
};
#endif

