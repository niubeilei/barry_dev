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
// 06/15/2011: Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"

#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "DataShuffler/Testers/TestSuite.h"
#include "DataShuffler/Testers/DocidShufflerTester.h"
#include "Timer/TimerMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			
int gAosShowNetTraffic = 1;	
int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	int index = 1;
	OmnApp::setConfig("config_norm.txt");
	OmnApp::appStart(argc, argv);
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

	AosTimerMgr::getSelf();
	AosXmlTagPtr testers = theApp.getAppConfig()->getFirstChild("testers");

	if (tries < 1) tries = 1000000;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DataShufflerTester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosDataShufflerTestSuite::getSuite(testers));
	
	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat().data() << endl;
	
	testMgr = 0;

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 

