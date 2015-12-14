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

#ifndef Aos_AosSecuredShell_Tester_SecuredShellTester_h
#define Aos_AosSecuredShell_Tester_SecuredShellTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosSecuredShellTester : public OmnTestPkg
{
	enum
	{
		eMaxShellId = 100
	};

private:
	int			mNumShells;
	OmnString	mShellIds[eMaxShellId];
	bool		mShellsCreated;

public:
	AosSecuredShellTester();
	~AosSecuredShellTester() {}

	virtual bool		start();

private:
	bool	testCli(int);
	bool	securedShellTest();
	bool	runCreateShell();
	bool	restoreCommands();
	bool	checkShellCommands(const OmnString &shellId);
};
#endif

