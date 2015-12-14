////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemCommandsTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTester_KtcpvsTester_h
#define Omn_CliTester_KtcpvsTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosSystemCommandsTester : public OmnTestPkg
{
private:

public:
	AosSystemCommandsTester()
	{
		mName = "AosSystemCommandsTester";
	}
	~AosSystemCommandsTester() {}

	virtual bool		start();

private:
	bool	testCli();
    bool    testSaveConfig();
    bool    testLoadConfig();
	bool	testShowConfig();
};
#endif

