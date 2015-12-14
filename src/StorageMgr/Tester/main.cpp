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
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SiteMgr/SiteMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageMgr/Tester/TestSuite.h"
#include "StorageMgr/StorageMgr.h"
#include "Util/OmnNew.h"
#include "util2/global_data.h"
#include "Util1/MemMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>



int 
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp theApp(argc, argv);
	OmnApp::setConfig("config_norm.txt");
	
	//OmnThread thread(pthread_self(), "main");
	//OmnThreadMgr::setThread(&thread);
	
	AosStorageMgr::getSelf();
	AosStorageMgr::getSelf()->config(OmnApp::getAppConfig());
	AosStorageMgr::getSelf()->start();

	int tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("StorageMgr/StorageMgrTester", "Try", "Ketty");
	testMgr->setTries(tries);
	OmnString test = "Storage";
	testMgr->addSuite(AosStorageMgrTestSuite::getSuite(test));

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


