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
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "Porting/GetTime.h"
#include "Porting/Process.h"
#include "XmlUtil/XmlTag.h"
#include "Porting/Sleep.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEUtil/Testers/TestSuite.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/TcpServerEpoll.h"
#include "XmlParser/XmlItem.h"
  

AosXmlTagPtr gAosAppConfig;
int gAosShowNetTraffic = 1;

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	OmnApp::setConfig("config_norm.txt");

	int index = 0;
	while (index < argc)
	{
		// if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		// {
		// 	tries = atoi(argv[index+1]);
		// 	index += 2;
		// 	continue;
		// }

		index++;
	}

	int max_alarms = 10;
	OmnAlarmMgr::setMaxAlarms(max_alarms);
	if (!gAosAppConfig)
	{
		AosXmlParser parser;
		OmnString str = "<config />";
		gAosAppConfig = parser.parse(str, "" AosMemoryCheckerArgs);
		aos_assert_r(gAosAppConfig, -1);
	}

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SEUtil/Tester", "Try", "Chen Ding");
	// testMgr->setTries(tries);
	testMgr->addSuite(AosSEUtilTestSuite::getSuite());



	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	int num = 0;
	while (1)
	{
		OmnSleep(1);
		if (num++ == 10)
		{
			// Print Epoll Server stat
			vector<OmnTcpServerEpollPtr> servers = OmnTcpServerEpoll::getEpollServers();
			if (servers.size() > 0)
			{
				for (u32 i=0; i<servers.size(); i++)
				{
					servers[i]->printStatus();
				}
			}
			num = 0;
		}
	}

	theApp.exitApp();
	return 0;
} 

