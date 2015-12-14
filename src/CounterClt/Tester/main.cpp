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
#include "Alarm/Alarm.h"

#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "CounterClt/Tester/TestSuite.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "SEUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Ptrs.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


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

	AosXmlTagPtr testers = theApp.getAppConfig()->getFirstChild("testers");

	OmnThread thread(pthread_self(), "main");
    OmnThreadMgr::setThread(&thread);

	if (tries < 1) tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("CounterClt", "Try", "Lynch Yang");
	testMgr->setTries(tries);
	testMgr->addSuite(AosCounterCltTestSuite::getSuite(testers));
	
	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat() << endl;
	testMgr = 0;
	theApp.exitApp();
	return 0;
} 

