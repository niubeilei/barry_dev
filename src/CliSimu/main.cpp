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
#include "aosUtil/Debug.h"
#include "AppMgr/App.h"
#include "CliSimuLib/KernelApiFunc.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelAPI/UserLandWrapper/TestTcpApiU.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>


void cliInterface();
extern "C" int tcp_vs_proc_cmd(int argc, char **argv); 

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
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	OmnKernelApi::setKernelApiFunc(AosKernelApi_setSockopt);
	OmnKernelApi::init();

	char c = 0;

	while (c != '0')
	{
		cliInterface();
		continue;
	}

	theApp.appLoop();
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
        line = readline("[ipacketengine]$");
    
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

extern int OmnKernelApi_init(void);

int AosCliSimu_kernelInit(void)
{
	OmnKernelApi_init();
	AosCertMgr_init();
	AosCertChain_init();
	return 0;
}

