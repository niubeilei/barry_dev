////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerTester.cpp
// Description:
// The secured shell supports the following commands:
//  1. Create shells
//  2. Restore commands
//  3. Start shell
//  4. Stop shell
//  5. Remove shells
//
// We will first randomly determine which command to run. 
//
// Run Create Shells
// If shells have already been created, running this command will generate
// an error. Otherwise, the shells should be created. 
//
// Run Restore Commands
// 1. If shells have not been created, we should get an error. 
// 2. If shells have been created, 
//    a. The shell commands were not restored before, it should be successful.
//    b. The shell commands were restored before, it should be successful.
//
// Start shell
// 1. If shells have not been created, this should get an error.
// 2. Otherwise, 
//    a. If the user has started a shell before, it should fail
//    b. If the user has not started any shell, it should suceed
//
// Stop shell
// 1. If shells have not been created, it will fail.
// 2. Otherwise, 
//    a. If the shell has not been started yet, it will fail. 
//    b. Otherwise, it should succeed. 
//
// Remove Active shells
// 1. If shells have not been created, it will fail.
// 2. Otherwise, it should remove all active shells. 
//
// Clear Shells
// 1. If shells have not been created, it will fail.
// 2. Otherwise, it should remove all active shells and clear the 
//    shell definition. 
//
//       
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SecuredShell/SecuredShellMgr.h"
#include "SecuredShell/SecuredShell.h"
#include "SecuredShell/ShellCommandMgr.h"
#include "SecuredShell/ShellCommand.h"
#include "SecuredShell/Tester/SecuredShellTester.h"

#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/RandomSelector.h"
#include "Porting/Sleep.h"


AosSecuredShellTester::AosSecuredShellTester()
:
mNumShells(0), 
mShellsCreated(false)
{
	mName = "AosSecuredShellTester";

	mShellIds[0] = "shell_1";
	mShellIds[1] = "shell_2";
	mNumShells = 2;
}


bool AosSecuredShellTester::start()
{
	testCli(1);
	// securedShellTest();
	return true;
}


bool
AosSecuredShellTester::testCli(int tries)
{
	while (tries)
	{
		// Determine which operations to run
		int cmd = OmnRandom::nextInt(0, 5);
		switch (cmd)
		{
		case 0:
			 runCreateShell();
			 break;

		case 1:
			 restoreCommands();
			 break;

		default:
			 OmnAlarm << "Invalid command Index: " << cmd << enderr;
		return false;
		}

		tries--;
	}

	return true;
}


bool 
AosSecuredShellTester::runCreateShell()
{
	//
	// Run Create Shells
	// If shells have already been created, running this command will generate
	// an error. Otherwise, the shells should be created. 
	//
	if (mShellsCreated)
	{
		aosRunCli("secured shell create shell", false);
	}
	else
	{
		aosRunCli("secured shell create shell", true);
		mShellsCreated = true;
	}

	return true;
}


bool
AosSecuredShellTester::restoreCommands()
{
	//
	// Run Restore Commands
	// 1. If shells have not been created, we should get an error. 
	// 2. If shells have been created, 
	//    a. The shell commands were not restored before, 
	//       it should be successful.
	//    b. The shell commands were restored before, it should be successful.
	//
	OmnString cmd;
	if (!mShellsCreated)
	{
		aosRunCli("secured shell restore commands all", false);

		for (int i=0; i<mNumShells; i++)
		{
			cmd = "secured shell restore commands ";
			cmd << mShellIds[i];

			aosRunCli(cmd, false);
		}

		return true;
	}

	int tries = 1000;
	OmnString shellid;
	char buff[100];

	while (tries)
	{
		// 
		// First, we will randomly determine whether to restore 
		// a valid shell, all, or invalid shell.
		//
		int option = OmnRandom::nextInt(0, 2);
		switch (option)
		{
		case 0:
			 aosRunCli("secured shell restore commands all", true);
			 checkShellCommands("all");
			 break;

		case 1:
			 option = OmnRandom::nextInt(0, mNumShells-1);
			 cmd = "secured shell restore commands ";
			 cmd << mShellIds[option];
			 aosRunCli(cmd, true);

			 checkShellCommands(mShellIds[option]);

			 break;

		case 2:
			 // 
			 // Restore an invalid shell id
			 //
			 OmnRandom::nextStr(buff, 1, 15);
			 for (int i=0; i<mNumShells; i++)
			 {
				if (mShellIds[i] == buff)
				{
					break;
				}
			 }

			 cmd = "secured shell restore commands ";
			 cmd << buff;
			 aosRunCli(cmd, false);
			 break;
		}

		tries--;
	}

	return true;
}


bool
AosSecuredShellTester::checkShellCommands(const OmnString &shellId)
{
	return true;
}


bool AosSecuredShellTester::securedShellTest()
{
	OmnBeginTest << "Test AosSecuredShell";
	mTcNameRoot = "SecuredShellTest";
	AosSecuredShellMgr mgr;
	int times = 1000 ;
	int minLen = 2;
	int maxLen = 10;
	char* randomStr1 = new char [maxLen];
	char* randomStr2 = new char [maxLen];
	while(times)
	{
		//test AosSecuredShellMgr::startShell();
		//	int		startShell(const OmnString &shellId, 
		//					const OmnString &username, 
		//					u32 duration);
		OmnRandom::nextStr(randomStr1,minLen, maxLen); 
		OmnRandom::nextStr(randomStr2,minLen, maxLen);
	//	mgr.startShell(randomStr1,randomStr2,10);
	//	bool ret =  mgr.stopShell(randomStr1,randomStr2);
		OmnString rslt;
		OmnString username;
		OmnString shellId;
		OmnString password;
		int duration;
		username << "irondoll";
		shellId << "shell_2";
		password << "123";
		duration = 123;
		bool ret = mgr.startSecuredShell(username,shellId,password,duration,rslt);
		//bool ret =  mgr.stopShell("abc","abc");
		OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		times--;	
	}	
	return true;
}
