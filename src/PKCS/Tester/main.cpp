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
#include "PKCS/Tester/PkcsTestSuite.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/ThreadDef.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"


    
int 
main(int argc, char **argv)
{	
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
		theApp.startSingleton(OmnNew OmnCliProcSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " 
			<< e.toString() << endl;
		return 0;
	}

	int index = 1;
	while (index < argc)
	{
		index++;
	}

	OmnKernelApi::init();

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SSL Tester", "Try", "Chen Ding");
    testMgr->addSuite(AosPkcsTestSuite::getSuite());

    cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

	theApp.appLoop();
	return 0;
} 


