////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashIntTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_HashIntTester_h
#define Omn_KernelUtil_HashIntTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosHashIntTester : public OmnTestPkg
{
private:

public:
	AosHashIntTester()
	{
		mName = "AosHashIntTester";
	}
	~AosHashIntTester() {}

	virtual bool		start();

private:
	bool	normalTest();
};
#endif

