////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ObjTester_ObjTestSuite_h
#define Omn_ObjTester_ObjTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnObjTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnObjTestSuite() {}
	~OmnObjTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

