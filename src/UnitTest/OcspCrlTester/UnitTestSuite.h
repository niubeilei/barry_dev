////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UnitTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_UnitTestSuite_h
#define Omn_UnitTest_UnitTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"

class OmnUnitTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnUnitTestSuite(); 
	virtual ~OmnUnitTestSuite();

	static OmnTestSuitePtr	getSuite(const OmnIpAddr &localAddr,
									 const OmnIpAddr &clientIp1, 
									 const OmnIpAddr &clientIp2, 
									 const OmnIpAddr &serverIp,	
									 const OmnIpAddr &bouncerIp, 
									 const OmnIpAddr &OcspCrlServerIp);
};

#endif

