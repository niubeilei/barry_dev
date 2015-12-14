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
#include "aos/aosKernelApi.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/KernelInterface.h"

#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"

#include "Util/OmnNew.h"
#include "JNS9054Card/Tester/JNS9054CardTestSuite.h"

int 
main(int argc, char **argv)
{	

	OmnApp theApp(argc, argv);
    try
    {
        theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
    }

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}
	
	OmnKernelApi::init();
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("JNS9054CardTester", "JNS9054Card api testing", "Hugo");
	testMgr->addSuite(OmnJNS9054CardTestSuite::getSuite());

	cout << "Start Testing ..." << endl;

 	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
	theApp.exitApp();
	return 0;

} 


