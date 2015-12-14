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
#include "UnitTest/OcspCrlTester/UnitTestSuite.h"
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
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <readline/readline.h>
#include <readline/history.h>

void cliInterface();
    
int 
main(int argc, char **argv)
{
	OmnIpAddr sslClient1("0.0.0.0");
	OmnIpAddr sslClient2("0.0.0.0");
	OmnIpAddr sslServer("0.0.0.0");
	OmnIpAddr bouncer("0.0.0.0");
	OmnIpAddr AppServer("0.0.0.0");
	OmnIpAddr localAddr("0.0.0.0");

	if (argc != 9)
	{
		cout << "command usage: OcspCrlTester -local <localaddr> -c1 <sslclientip1> -c2 <sslclientip2> -ser <sslserverip> \n" << endl;
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
		
		if (strcmp(argv[index], "-c1") == 0)
		{
			sslClient1 = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-c2") == 0)
		{
			sslClient2 = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-ser") == 0)
		{
			sslServer = OmnIpAddr(argv[index+1]);
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

	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("OcspCrlTester", "Ocsp Crl integration testing", "Hugo"); 
 	// client ip1;
 	// client ip2; 
	// server ip; 
	// bouncer ip; 
	// ocsp crlserver ip
    testMgr->addSuite(OmnUnitTestSuite::getSuite(localAddr,sslClient1, sslClient2, sslServer, OmnIpAddr("192.168.1.60"), OmnIpAddr("192.168.1.60")));
    
	cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat()<< endl;

    testMgr = 0;

//	cliInterface();

//	theApp.appLoop();
	theApp.exitApp();

	return 0;
} 

void
cliInterface()
{
	OmnString rslt;                                                                                                        
    bool skip = false;                                                                                                      
    char *line;                                                                                         

    while (1)                                                                                                         
    {                                                                                                      
        line = readline("[OCSPCRLTESTING_SSLProxy]$");  
                                                                                                           
        if (strcmp(line, "")!=0)                                                                           
        {                                                                                                              
            add_history(line);                                                                                              
        }                                                                                                                   
                                                                                                                  
        rslt = "";                                                                                                          
                                                                                                                        
        if (strlen(line) > 3)                                                                                                
        {                                                                                                                      
            skip = false;                                                                                                 
            OmnCliProc::getSelf()->runCli(line, rslt);                                                            
            cout << rslt << endl;                                                                                         
        }                                                                                                                    
        else                                                                                                          
        {                 
            skip = true;                                                                                                    
        }
                                                                                 
        free(line);                                                                                                     
    }    
}
 
