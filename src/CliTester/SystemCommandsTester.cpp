////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemCommandsTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliTester/SystemCommandsTester.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool AosSystemCommandsTester::start()
{
	// 
	// Test default constructor
	//
	testSaveConfig();
	testLoadConfig();
	testShowConfig();
//	integrateTest();

	return true;
}

	//
	// This test file includes three test cases at least as below:
	//    Test Case 1: Save Configurations
	//    Test Case 2: Load Configurations
	//    Test Case 3: Show System Configurations
	//

bool AosSystemCommandsTester::testSaveConfig()
{
	OmnString rslt;
	OmnString cmd;

	OmnBeginTest << "Test System CLI SaveConfig";
	mTcNameRoot = "Test_CLI_SaveConfig";

	// 
	// Test Case 1: Save Configurations
	//  a. without a configuration file-name
	//  b. with a configuration file-name
	//
   
	//  a. without configuration file-name
    
	aosRunCli("save config",true);

	//  b. with configuration file-name
    //   b.1. file-name with underline
	//   b.2. file-name with a blank
    //   b.3. file-name with a number&letter&underline compounded
	//   b.4. file-name with path

	for (int i = 1; i <= 5000; i++)
    {
        char local[200];
        sprintf(local, "app proxy add app%d http 192.168.1.81 %d ssl",
            i, 5000+i);

            aosRunCli(local, true);
    }

	aosRunCli("save config testfile",true); // default path
	aosRunCli("save config test_file",true); 
	aosRunCli("save config test file",true); 
	aosRunCli("save config test1_file#_Shift",true); 
	aosRunCli("save config /tmp/test_file",true); // contain the path   
    
	return true;
}
	
bool AosSystemCommandsTester::testLoadConfig()
{
	OmnString rslt;
	OmnString cmd;

	OmnBeginTest << "Test System CLI LoadConfig";
	mTcNameRoot = "Test_CLI_LoadConfig";

	// 
	// Test Case 2: Load Configurations
	//  a. to process 'load config' without a configuration file
	//  b. to process 'load config' in a fresh system with a configuration file
    //   b.1. file-name with underline
	//   b.2. file-name with a blank
    //   b.3. file-name with a number&letter&underline compounded
	//   b.4. file-name with path
	//	c. to process 'load config' not in a fresh system with a configuration file
	//		i.e. there have been configuration which conflict with settings just now	
	//
   
    //  a. to process load config without a configuration file

    aosRunCli("load config",true);

	//  b. to process load config with a configuration file in a fresh system

	aosRunCli("load config testfile",true); // default path
	aosRunCli("load config test_file",true); 
	aosRunCli("load config test file",false); 
	aosRunCli("load config test1_file#_Shift",true); 
	aosRunCli("load config /tmp/test_file",true); // contain the path   

	//	c. tp process 'load config' with a configuration file not in a fresh system

	aosRunCli("app proxy add app1 http 10.1.1.1 3000 ssl",true);
	aosRunCli("app proxy add app2 http 10.1.1.2 3200 plain",true);
	aosRunCli("forward table entry add app1 ww1.testfor.cm 3000 ssl",true);
	aosRunCli("forward table entry add app2 ww2.testfor.cm 3200 plain",true);
	aosRunCli("load config testfile",true);
	aosRunCli("load config test_file#",true);
	aosRunCli("load config testfile",true);
	

    return true;
}

	
bool AosSystemCommandsTester::testShowConfig()
{
	OmnString rslt;
	OmnString cmd;

	OmnBeginTest << "Test System CLI ShowConfig";
	mTcNameRoot = "Test_CLI_ShowConfig";

 
	aosRunCli_getStr("config show",true,rslt);
    return true;
}

