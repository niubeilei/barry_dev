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

#include "QoS/QoSTester/QosTestSuite.h"
#include "QoS/QoSTester/QosTestThread.h"
#include "QoS/qos_rule_cli.h"
#include "QoS/qos_rule.h"
#include "QoS/qos_global.h"
#include "KernelSimu/dev.h"		// dev_init()

#include <readline/readline.h>
#include <readline/history.h>

void cliInterface();
int OmnKernelApi_addCliCmd(char *id, aosCliFunc func);
	
static int Qos_Init(void)
{
	//AosQoSTestThread*  testThread = new AosQoSTestThread();
	
	OmnKernelApi_addCliCmd("log_setfilter", AosTracer_setFilterCli);
	OmnKernelApi_addCliCmd("log_show", AosTracer_showCli);
	OmnKernelApi_addCliCmd("module_show", AosModule_showCli);
			   
	dev_init();
	aosqos_init();
	//testThread->start();
	return 0; 
}

int 
main(int argc, char **argv)
{	
	int index = 1;
	OmnApp theApp(argc, argv);
	OmnKernelApi_init();
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

    OmnKernelApi::setKernelApiFunc(AosKernelApi_setSockopt);
	OmnKernelApi::init();

	Qos_Init();

    cliInterface();

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("QoS Channel Management Tester", "Try", "Tim");
    testMgr->addSuite(AosQoSTestSuite::getSuite());

    cout << "Start QoS Channel Management Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

	theApp.appLoop();
	return 0;
} 


void cliInterface()
{
    const int cmdlen = 1000;
    char *cmd;
    OmnString rslt;
    bool skip = false;

    while (1)
    {
		cmd = readline("(packetengine)#");

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

		add_history(cmd);
		free(cmd);
    }
}


int AosCliSimu_kernelInit(void)
{
    OmnKernelApi_init();
    return 0;
}

