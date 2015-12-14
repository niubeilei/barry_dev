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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_Testers_TestSuite_h
#define Aos_DataBlob_Testers_TestSuite_h

#include "Rundata/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosDataBlobTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDataBlobTestSuite() {}
	~AosDataBlobTestSuite() {}

	static OmnTestSuitePtr		getSuite(const OmnString &type);
};
#endif

