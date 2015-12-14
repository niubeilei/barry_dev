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
// 03/18/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlInterface_Testers_XmlInterfaceTestSuite_h
#define Aos_XmlInterface_Testers_XmlInterfaceTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosXmlInterfaceTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosXmlInterfaceTestSuite() {}
	~AosXmlInterfaceTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

