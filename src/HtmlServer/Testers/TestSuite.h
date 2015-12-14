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
// 2011/01/15	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlServer_Testers_TestSuite_h
#define Aos_HtmlServer_Testers_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosHtmlServerTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosHtmlServerTestSuite() {}
	~AosHtmlServerTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

