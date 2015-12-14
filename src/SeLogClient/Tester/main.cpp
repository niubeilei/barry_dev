////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 06/03/2011: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DocClient/DocClient.h"
#include "IdGen/IdGenMgr.h"
#include "IILClient/IILClient.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEModules/SeRundata.h"
#include "SiteMgr/SiteMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "SeLogClient/Tester/TestSuite.h"
#include "SeLogClient/Tester/SeLogClientTester.h"
#include "Util/OmnNew.h"
#include "util2/global_data.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Util1/MemMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int gAosLogLevel = 1;
#include <dirent.h>


int 
//clientMain(int argc, char **argv)
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp::appStart(argc, argv);
	
	int tries;
	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-create") == 0)
		{
		//	AosSeLogClientTester::setCreate(true);
			index++;
		    continue;
		}
		if (strcmp(argv[index], "-version") == 0)
		{
		//	AosSeLogClientTester::setCheckVersion(true);
			index++;
		    continue;
		}
		if (strcmp(argv[index], "-tries") == 0)
		{
			tries = atoi(argv[index+1]);
			index ++;
			continue;
		}
		index ++;
	}


	OmnApp::setConfig("config_norm.txt");
	
	OmnApp theApp(argc, argv);	
	
cout << __FILE__ << ":" << __LINE__ << " main thread: " << pthread_self() << endl;
	// OmnThread thread(pthread_self(), "main");
	// OmnThreadMgr::setThread(&thread);

	try
	{
		AosSengAdmin::getSelf();
		AosDocClient::getSelf();
		AosIILClient::getSelf();
	}
	catch(...)
	{
		OmnAlarm << "Failed to start application!" << enderr;
		theApp.exitApp();
		return 0;
	}

	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SeLogClient/SeLogClientTester", "Try", "Tom");
	testMgr->setTries(tries);
	OmnString test = "SeLogClient";
	testMgr->addSuite(AosSeLogClientTestSuite::getSuite(test));

	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 


