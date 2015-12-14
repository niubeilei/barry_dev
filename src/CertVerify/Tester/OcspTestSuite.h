////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Ocsp_OcspTesterSuite_h
#define Omn_Ocsp_OcspTesterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class OmnOcspTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnOcspTestSuite() {}
	~OmnOcspTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

