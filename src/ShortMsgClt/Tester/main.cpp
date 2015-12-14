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
// 05/14/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgClt/Tester/TestSuite.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Porting/Sleep.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"

int gAosLogLevel;

int gAosShowNetTraffic = 1;

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	int index = 1;
	OmnApp::setConfig("config_norm.txt");
	int tries = 0;

	while (index < argc)
	{
		if (strcmp(argv[index], "-tries") == 0)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		index++;
	}

	OmnThread thread(pthread_self(), "main");
	OmnThreadMgr::setThread(&thread);

	AosXmlTagPtr testers = theApp.getAppConfig();

	if (tries < 1) tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("ShortMsgCltTester", "Try", "Brian Zhang");
	testMgr->setTries(tries);
	testMgr->addSuite(AosShortMsgCltTestSuite::getSuite(testers));
	
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

