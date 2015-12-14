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
#include "SystemCli/QuotaTester/QuotaCliTesterSuite.h"
#include <readline/readline.h>
#include <readline/history.h>

int sgCount=1;
int 
main(int argc, char **argv)
{	
	int index = 1;
	int index1=1;
	while(index1<argc)
	{	
		if(strcmp(argv[index1],"-h")==0)
		{
			cout<<"==================================== HELPS ===================================================================="<<endl;			
			cout<<"Usage         :  ./QuotaCliTester.exe [-t [MaxRunCount]] [-h]"<<endl;
			cout<<"get usage help:  ./QuotaCliTester.exe -h "<<endl;
			cout<<"-----------------------------paramater explations -----------------------------------------------------------"<<endl;
			cout<<"            -t:   Control the total run count of this torturter, by default, it value is 1"<<endl;
			cout<<"            -h:   show this help contents  "<<endl;
			cout<<"---------------------------------- examples ---------------------------------------------------------------"<<endl;
			cout<<"./QuotaCliTester.exe                       (this application will run only 1 time) "<<endl;
			cout<<"./QuotaCliTester.exe  -t 100               (this application will tun 100 times)"<<endl;
			cout<<""<<endl;
			cout<<"==============================================================================================================="<<endl;
			exit(0);
		}
		if(strcmp(argv[index1],"-t")==0)
		{
			sgCount=atoi(argv[++index1]);
			index1++;
			continue;
		}
		
	}
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

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Quota Tester", "Try", "xyb");

    testMgr->addSuite(QuotaCliTesterSuite::getSuite());

    cout << "Start Inet Testing ..." << endl;

    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;

	theApp.appLoop();
	return 0;
} 


