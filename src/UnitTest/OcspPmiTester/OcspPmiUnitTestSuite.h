////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspPmiUnitTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_PmiTester_OcspPmiUnitTestSuite_h
#define Omn_UnitTest_PmiTester_OcspPmiUnitTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"

class OmnOcspPmiUnitTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnOcspPmiUnitTestSuite(); 
	virtual ~OmnOcspPmiUnitTestSuite();

	static OmnTestSuitePtr	getSuite(const OmnIpAddr &clientIp, 
									 const OmnIpAddr &serverIp,
									 const OmnIpAddr &bouncerIp,
									 const OmnIpAddr &PMIServerIp);
};

#endif

