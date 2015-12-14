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

#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "KernelAPI/KernelAPI.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/GetTime.h"
#include "Proxy/Tester/TcpProxyTestSuite.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
  
   

int 
main(int argc, char **argv)
{
	
    OmnIpAddr 	clientAddr("192.168.2.151");
    OmnIpAddr 	serverAddr;
    OmnIpAddr 	localAddr("192.168.2.151");
    OmnIpAddr 	bouncerAddr;
	int			numOfTries = 1;
	int			clientPort = 80;
	int			connNum = 1;
	int			repeat  = 1;
	int			blockSize  = 1000;
    
    int index = 1; 
    while (index < argc)
    {   
        if (strcmp(argv[index], "-client") == 0)
        {   
            clientAddr = OmnIpAddr(argv[index+1]);
            index += 2;
            continue;
        }
        
        if (strcmp(argv[index], "-server") == 0)
        {   
            serverAddr = OmnIpAddr(argv[index+1]);
            index += 2;
            continue;
        }
		
        if (strcmp(argv[index], "-local") == 0)
        {   
            localAddr = OmnIpAddr(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-bouncer") == 0)
        {   
            bouncerAddr = OmnIpAddr(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-port") == 0)
        {   
            clientPort = atoi(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-conn") == 0)
        {   
            connNum = atoi(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-repeat") == 0)
        {   
            repeat = atoi(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-size") == 0)
        {   
            blockSize = atoi(argv[index+1]);
            index += 2;
            continue;
        }

        if (strcmp(argv[index], "-trynum") == 0)
        {   
            numOfTries = atoi(argv[index+1]);
            index += 2;
            continue;
        }
        
        
        if (strcmp(argv[index], "--help") == 0)
        {   
        	cout << endl;
        	cout << "For Example:" << endl;
			cout << "Note that if client port number is under 1024,please login as root." << endl;
        	cout << endl;
        	cout << "Tester.exe -client 192.168.1.1 -port 8001 -local 192.168.1.3  -trynum 1 -conn 1 -repeat 1 -size 100" << endl;
        	cout << endl;
        	cout << endl;
        	exit(0);
			return 0;
        }
        
        index++;
	}
	
	OmnRandom r1;
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnTimeDriverSingleton());
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	OmnKernelApi::init();

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("UtilTester", "Try", "Chen Ding");
	testMgr->addSuite(AosProxyTestSuite::getSuite(clientAddr,
											      serverAddr,
											      localAddr,
											      bouncerAddr,
								     			  clientPort,								     
								     			  numOfTries,
												  connNum,
												  repeat,
												  blockSize));

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 
