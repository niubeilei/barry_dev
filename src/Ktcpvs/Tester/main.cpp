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
#include "CliSimuLib/KernelApiFunc.h"
#include "AppMgr/App.h"
#include "Ktcpvs/Tester/KtcpvsTester.h"
#include "Ktcpvs/Tester/KtcpvsTestSuite.h"
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

bool AosKtcpvsTesterIsGeneratorFlag = false;
bool AosKtcpvsTesterIsBouncerFlag = false;

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
		cout << "****** Failed to start the application: " 
			<< e.toString() << endl;
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}
	
//	if( argc != 4) 
//	{
//		cout << "please fllow this format:\ne.g. KtcpvsTester.exe <[-gen/-boun]> <-repeat> <-conns> <-len>" << endl;
//		perror("Command Line:");
//		exit(0);
//	}

	int index = 1;
    while (index < argc)
    {
        if (strcmp(argv[index], "-gen") == 0)
        {
        	AosKtcpvsTesterIsGeneratorFlag = true;
         }

		if (strcmp(argv[index], "-boun") == 0)
		{
			AosKtcpvsTesterIsBouncerFlag = true;	
		}

        if (strcmp(argv[index], "-repeat") == 0)
        {
            AosKtcpvsTester::setRepeat(atoi(argv[++index])); 
        }

        if (strcmp(argv[index], "-conns") == 0)
        {
			AosKtcpvsTester::setConns(atoi(argv[++index]));
        }

        if (strcmp(argv[index], "-len") == 0)
        {
            AosKtcpvsTester::setLen(atoi(argv[++index]));
        }
	
        index++;
    }

	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("KtcpvsTester", "Try", "Hugo");
	testMgr->addSuite(OmnKtcpvsTestSuite::getSuite());

	cout << "Start Testing ..." << endl;

 	testMgr->start();

	cout << "\nFinished. " <<endl;

	testMgr = 0;
	theApp.exitApp();
	return 0;

} 


