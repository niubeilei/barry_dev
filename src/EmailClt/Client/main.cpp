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
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/XmlDoc.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "EmailClt/Client/TestSuite.h"
#include "Util/OmnNew.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void aosSignalHandler(int value)
{
}

int test123() {return 0;}

int gAosLogLevel = 1;
#include <dirent.h>


int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	OmnApp::setConfig("config_norm.txt");
	int tries = 10;

	OmnThread thread(pthread_self(), "main");
	OmnThreadMgr::setThread(&thread);

	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Email", "Try", "Brian");
	testMgr->setTries(tries);
	OmnString index = OmnString(argv[1]);
	testMgr->addSuite(AosTransClientTestSuite::getSuite(index));

	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
	while (1)
	{
		OmnSleep(10000);
	}

	return 0;
} 


