////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.h
// Description:
//   
//
// Modification History:
// 2012/03/07	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_Testers_TestSuite_h
#define Aos_DataFormat_Testers_TestSuite_h

#include "SEUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDataFormatTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDataFormatTestSuite() {}
	~AosDataFormatTestSuite() {}

	static OmnTestSuitePtr getSuite(const AosXmlTagPtr &testers);
};
#endif

