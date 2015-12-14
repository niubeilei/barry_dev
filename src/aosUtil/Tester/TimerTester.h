////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_TimerTester_h
#define Aos_AosUtil_Tester_TimerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosTimerTester : public OmnTestPkg
{
private:

public:
	AosTimerTester()
	{
		mName = "AosTimerTester";
	}
	~AosTimerTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

