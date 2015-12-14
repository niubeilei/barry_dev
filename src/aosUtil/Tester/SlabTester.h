////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SlabTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_SlabTester_h
#define Aos_AosUtil_Tester_SlabTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosSlabTester : public OmnTestPkg
{
private:

public:
	AosSlabTester()
	{
		mName = "AosSlabTester";
	}
	~AosSlabTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	testKernel();
};
#endif

