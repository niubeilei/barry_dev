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
// 2014/10/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturers_XmlDocTorturer_TestSuite_h
#define Omn_Torturers_XmlDocTorturer_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosXmlDocTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosXmlDocTestSuite() {}
	~AosXmlDocTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

