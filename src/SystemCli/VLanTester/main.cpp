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
#include "CliSimuLib/KernelApiFunc.h"
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
#include "aos/aosKernelApi.h"

#include "aosUtil/Tracer.h"
#include "SystemCli/VLanTester/VLanTestSuite.h"
#include "SystemCli/VLanTester/VLanTester.h"
#include <readline/readline.h>
#include <readline/history.h>


int 
main(int argc, char **argv)
{	
	int index = 1;
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

//    OmnKernelApi::setKernelApiFunc(AosKernelApi_setSockopt);
//	OmnKernelApi::init();

//	OmnKernelApi_init();


    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("VLan", "Try", "dxr");

    testMgr->addSuite(AosVLanTestSuite::getSuite());

    cout << "Start VLan Testing ..." << endl;

	testMgr->setSeed(argc,argv);
    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

	theApp.appLoop();
	return 0;
} 



