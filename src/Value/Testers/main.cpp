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
//
// Modification History:
// 2013/03/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Config/ConfigMgr.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/ExeReport.h"
#include "Util/MemoryChecker.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "Value/Testers/TestSuite.h"
#include "XmlParser/XmlItem.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
using namespace std;
  
 

int 
main(int argc, char **argv)
{
	AosMemoryChecker::getSelf();
	cout << "Current thread id: " << OmnGetCurrentThreadId() << endl;

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp theApp(argc, argv);
	AosConfigMgr config_mgr;

	int index = 1;
	int tries = 0;
	int num_threads = 0;
	AosXmlTagPtr app_config;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-wn") == 0 && index < argc-1)
		{
			int max_alarms = atoi(argv[index+1]);
			index += 2;
			OmnAlarmMgr::setMaxAlarms(max_alarms);
			continue;
		}

		if (strcmp(argv[index], "-tries") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-threads") == 0 && index < argc-1)
		{
			num_threads = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		index++;
	}

	if (!app_config) app_config = OmnApp::readConfig("config_norm.txt");
	aos_assert_r(app_config, -1);
	OmnApp::setConfig(app_config);

	if (app_config)
	{
		AosXmlTagPtr tag = app_config->getFirstChild("value_tester");
		if (tag)
		{
			if (tries > 0) tag->setAttr("tries", tries);
			if (num_threads > 0) tag->setAttr("num_threads", num_threads);
		}
	}

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Value/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosValueTestSuite::getSuite("all"));

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
 
	AosExeReport report(10);
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

