////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliComm_Tester_CliCommTester_h
#define Omn_CliComm_Tester_CliCommTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosCliCommTester : public OmnTestPkg
{
private:

public:
	AosCliCommTester()
	{
		mName = "AosCliCommTester";
	}
	~AosCliCommTester() {}

	virtual bool		start();

private:
	bool	testCli();
};
#endif

