////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemoryTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_MemoryTester_h
#define Aos_AosUtil_Tester_MemoryTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosMemoryTester : public OmnTestPkg
{
private:

public:
	AosMemoryTester()
	{
		mName = "AosMemoryTester";
	}
	~AosMemoryTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	testKernel();
	bool	testLeak();
};
#endif

