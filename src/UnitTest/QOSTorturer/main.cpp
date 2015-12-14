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
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/ThreadDef.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "UnitTest/QOSTorturer/QOSTestSuite.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <readline/readline.h>
#include <readline/history.h>

void cliInterface();
    
int 
main(int argc, char **argv)
{	
    OmnIpAddr proxyServer("0.0.0.0");
    OmnIpAddr bouncer("0.0.0.0");
    OmnIpAddr AppServer("0.0.0.0");
    OmnIpAddr localAddr("0.0.0.0");

    if (argc != 11)
    {
        cout << "command usage: qostorturer -local <localAddr> -ser <proxyserverip>\n" << endl;
        exit(0);
    }

    int index = 1;
    while (index < argc)
    {
        if (strcmp(argv[index], "-local") == 0)
        {
            localAddr = OmnIpAddr(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-ser") == 0)                                                                               
        {                                                                                                                   
            proxyServer = OmnIpAddr(argv[index+1]);                                                                           
            index += 2;                                                                                                     
            continue;                                                                                                       
        }                                                                                                                   
                                                                                                                            
        if (strcmp(argv[index], "-b") == 0)                                                                                 
        {                                                                                                                   
            bouncer = OmnIpAddr(argv[index+1]);                                                                             
            index += 2;                                                                                                     
            continue;                                                                                                       
        }                                                                                                                   
                                                                                                                            
        if (strcmp(argv[index], "-apps") == 0)                                                                              
        {                                                                                                                   
            AppServer = OmnIpAddr(argv[index+1]);                                                                           
            index += 2;                                                                                                     
            continue;                                                                                                       
        }                                                                                                                   
                                                                                                                            
        index++;                                                                                                            
    }                                                 

	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
		theApp.startSingleton(OmnNew OmnCliProcSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

    OmnKernelApi::init();

	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("QOSTorturer", "Web Wall unit testing", "Shawn Lee");  
    testMgr->addSuite(OmnQOSTestSuite::getSuite(	localAddr,
    													proxyServer,
    													OmnIpAddr("192.168.1.245")));// pmi server ip

    cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat()<< endl;

	cout << "**************************************" << endl;
    testMgr = 0;
    
	theApp.appLoop();
	theApp.exitApp();

	return 0;

} 


