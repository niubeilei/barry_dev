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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "util2/global_data.h"
#include "Util/OmnNew.h"
#include "Util/MemoryChecker.h"
#include "Util1/MemMgr.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


#include <openssl/sha.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "DataFormat/Tester/TestSuite.h"


int gAosLogLevel = 1;
#include <dirent.h>

int 
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp::setFrontEndServer(true);
	OmnApp::setConfig("config_norm.txt");
	
	if (!OmnApp::getAppConfig())
	{
		cout << "Faild to get config! " << endl;
		exit(-1);
	}

	OmnApp theApp(argc, argv);
	AosXmlTagPtr test = OmnApp::getAppConfig()->getFirstChild("tester");
	if (!test)
	{
		cout << "Missing tester Type!";
		return false;
	}

	int tries = test->getAttrInt("tries", 1000);
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DataFormat", "Try", "Ken Lee");
	testMgr->setTries(tries);
	testMgr->addSuite(AosDataFormatTestSuite::getSuite(test));
	
	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat().data() << endl;
	testMgr = 0;
	
	while (1)
	{
		OmnSleep(1000);
	}

	theApp.exitApp();
	return 0;
} 


