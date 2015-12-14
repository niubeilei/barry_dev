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
// Modification History:
// 2015-11-20 Created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_TesterStmInterface_TestSuite_h
#define Aos_BlobSE_TesterStmInterface_TestSuite_h

#include "Tester/Ptrs.h"
#include "Tester/TestPkg.h"

class AosBlobSEStmInterfaceTestSuite:public OmnTestPkg
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosBlobSEStmInterfaceTestSuite() {}
	~AosBlobSEStmInterfaceTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

