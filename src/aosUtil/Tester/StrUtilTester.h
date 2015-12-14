////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrUtilTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_StrUtilTester_h
#define Aos_AosUtil_Tester_StrUtilTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosStrUtilTester : public OmnTestPkg
{
private:

public:
	AosStrUtilTester()
	{
		mName = "AosStrUtilTester";
	}
	~AosStrUtilTester() {}

	virtual bool		start();

private:
	bool	binIncTest();
};
#endif

