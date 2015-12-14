////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FWTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_FWTorturer_FWTestSuite_h
#define Omn_UnitTest_FWTorturer_FWTestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"

class OmnFWTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	OmnFWTestSuite(); 
	virtual ~OmnFWTestSuite();

	static OmnTestSuitePtr	getSuite(const OmnIpAddr &localAddr,
									 const OmnIpAddr &serverIp,
									 const OmnIpAddr &bouncerIp);
};

#endif

