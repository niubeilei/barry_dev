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
#include "ssl2/Tester/SslTestSuite.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "Porting/ThreadDef.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/cavium.h"


bool AosSSLTesterIsServerFlag = false;
bool AosSSLTesterIsClientFlag = false;
bool AosSSLTesterIsClientAuthFlag = false;
bool AosSSLTesterIsCaviumCardFlag = false;
    
void cliInterface();

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
		if (strcmp(argv[index], "-server") == 0)
		{
			AosSSLTesterIsServerFlag = true;
		}

		if (strcmp(argv[index], "-client") == 0)
		{
			AosSSLTesterIsClientFlag = true;
		}

		if (strcmp(argv[index], "-cltauth") == 0)
		{
			AosSSLTesterIsClientAuthFlag = true;
		}

		if (strcmp(argv[index], "-cavium") == 0)
		{
			AosSSLTesterIsCaviumCardFlag = true;
		}

		index++;
	}

    OmnKernelApi::setKernelApiFunc(AosKernelApi_setSockopt);
	OmnKernelApi::init();


    cliInterface();

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SSL Tester", "Try", "Chen Ding");
    testMgr->addSuite(AosSslTestSuite::getSuite());

    cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

    //char c = 0;
    //while (c != '0')
    //{
        //cliInterface();
        //continue;
    //}

	theApp.appLoop();
	if(AosSSLTesterIsCaviumCardFlag)
	{
		Csp1Shutdown();
	}
	return 0;
} 


void cliInterface()
{
    const int cmdlen = 1000;
    char cmd[cmdlen];
    OmnString rslt;
    bool skip = false;

    while (1)
    {
        if (!skip)
        {
            cout << "(PacketEngine)# " << flush;
        }

        int index = 0;
        while (index < cmdlen)
        {
            // char c = getchar();
            char c;
            read(1, &c, 1);
            if (c == '\n' || c == EOF)
            {
                cmd[index++] = 0;
                break;
            }
            cmd[index++] = c;
        }

        rslt = "";

        if (strlen(cmd) > 3)
        {
            skip = false;
			if (strcmp(cmd, "exit") == 0 )
				break;
            OmnCliProc::getSelf()->runCli(cmd, rslt);
            cout << rslt << endl;
        }
        else
        {
            skip = true;
        }
    }
}


int AosCliSimu_kernelInit(void)
{
    OmnKernelApi_init();
    AosCertMgr_init();
    AosCertChain_init();
    return 0;
}

