////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_RandomTester_h
#define Aos_AosUtil_Tester_RandomTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosRandomTester : public OmnTestPkg
{
private:

public:
	AosRandomTester()
	{
		mName = "AosRandomTester";
	}
	~AosRandomTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	check(char *file, int line, char *buff, 
				const u32 start, const u32 end);
};
#endif

