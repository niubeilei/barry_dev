////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SinglePmiUnitTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_PmiTester_SinglePmiUnitTestSuite_h
#define Omn_UnitTest_PmiTester_SinglePmiUnitTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"

class OmnSinglePmiUnitTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnSinglePmiUnitTestSuite(); 
	virtual ~OmnSinglePmiUnitTestSuite();

	static OmnTestSuitePtr	getSuite(const OmnIpAddr &localAddr,
									 const OmnIpAddr &clientIp1,
									 const OmnIpAddr &clientIp2,
									 const OmnIpAddr &clientIp3, 
									 const OmnIpAddr &serverIp,
									 const OmnIpAddr &bouncerIp,
									 const OmnIpAddr &PMIServerIp);
};

#endif

