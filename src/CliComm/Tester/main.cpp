////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include "CliComm/Tester/CliCommTestSuite.h"


    
int 
main(int argc, char **argv)
{	
	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("UtilTester", "Try", "Chen Ding");
    testMgr->addSuite(AosCliCommTestSuite::getSuite());

    cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

	theApp.appLoop();
	return 0;
} 


