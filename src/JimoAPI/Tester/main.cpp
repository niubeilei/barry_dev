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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "alarm_c/alarm.h"
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/XmlTag.h"
  
 
int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	OmnAlarmMgr::config();

	AosXmlTagPtr app_config;

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			aos_assert_r(app_config, false);
			OmnApp::setConfig(app_config);
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
		
		index++;
	}

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("JimoAPI/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	// testMgr->addSuite(OmnUtilTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

