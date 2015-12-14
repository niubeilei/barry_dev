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
// 06/15/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocLock_Torturer_TestSuite_h
#define Aos_DocLock_Torturer_TestSuite_h

#include "XmlUtil/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDocLockTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosDocLockTestSuite() {}
	~AosDocLockTestSuite() {}

	static OmnTestSuitePtr getSuite(const AosXmlTagPtr &testers);
};
#endif

