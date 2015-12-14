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
// 02/24/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_IpAddrTester_h
#define Omn_TestUtil_IpAddrTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnIpAddrTester : public OmnTestPkg
{
private:
	int		mTries;

public:
	OmnIpAddrTester();
	~OmnIpAddrTester() {}

	virtual bool		start();

private:
	bool	testIsValidMask();
};
#endif

