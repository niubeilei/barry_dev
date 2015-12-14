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
// 02/22/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_RandomTester_h
#define Omn_TestUtil_RandomTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnRandomTester : public OmnTestPkg
{
private:
	int		mTestNextIntTries;
	int		mTestNextIncorrectIpTries;
	int		mTestNextIpTries; 
	int		mTries;

public:
	OmnRandomTester();
	~OmnRandomTester() {}

	virtual bool		start();

private:
	bool	testNextInt();
	bool	testNextIncorrectIp();
	bool	testNextIP();
	bool	testNextMask();
	bool 	testNextTimeStr(const u32 tries);
	bool 	testNextInvalidTimeStr(const u32 tries);
};
#endif

