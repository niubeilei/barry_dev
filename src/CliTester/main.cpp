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
#include "CliTester/CliTestSuite.h"
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


void skbTest();
void bridgeTest();
void cliInterface();

static bool sg_userland_flag = false;
    
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
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

    int index = 1;
    while (index < argc)
    {
        if (strcmp(argv[index], "-userland") == 0)
        { 
            sg_userland_flag = true;
        }

        index++;
    }

	if (sg_userland_flag)
	{
    	OmnKernelApi::setKernelApiFunc(AosKernelApi_setSockopt);
	}
	OmnKernelApi::init();

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("CLITester", "Try", "Hugo Gong");
    testMgr->addSuite(OmnCliTestSuite::getSuite());

    cout << "Start Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

	cliInterface();

	theApp.appLoop();
	return 0;
} 


void
cliInterface()
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
			char c = getchar();
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

