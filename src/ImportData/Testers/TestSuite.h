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
// 03/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ImportData_Testers_TestSuite_h
#define Aos_ImportData_Testers_TestSuite_h

#include "Rundata/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosImportDataTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosImportDataTestSuite() {}
	~AosImportDataTestSuite() {}

	static OmnTestSuitePtr		getSuite(OmnString &type);
	static bool createData(
						const AosXmlTagPtr &config,
						const AosRundataPtr &rdata);
	static bool createDataNorm(
						const AosXmlTagPtr &config,
						const AosRundataPtr &rdata);
};
#endif

