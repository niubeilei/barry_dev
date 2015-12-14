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
// 2013/05/18 Creaed by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "DocMgr/Tester/TestSuite.h"
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Timer/Timer.h"
#include "Util/Tester/UtilTestSuite.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/ExeReport.h"
#include "Util/MemoryChecker.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "Util/Tester/ValueRsltTester.h"
#include "XmlParser/XmlItem.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <dirent.h>
#include <signal.h>
using namespace std;
  
 
void aosSignalHandler(int value)
{
	if (value == 18) 
	{
		OmnScreen << "To set ignored alarms" << endl;
		OmnApp::checkIgnoredAlarms();
	}
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			// Chen Ding, 06/07/2012
int gAosShowNetTraffic = 1;			// Chen Ding, 06/08/2012

bool raiseAlarmTest()
{
	aos_assert_r(false, false);
	return true;
}

int 
main(int argc, char **argv)
{
	signal(18, aosSignalHandler);

	OmnApp theApp(argc, argv);


	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
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
		
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	OmnAlarmMgr::config();
	AosSetErrorU(rdata, "first alarm testing") << enderr;
	AosSetErrorU(rdata, "second alarm testing") << enderr;
	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosDocMgrTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	// testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
 
	AosExeReport report(10);
	while (1)
	{
		OmnApp::checkIgnoredAlarms();
		OmnSleep(1);
		OmnAlarm << "Testing next" << enderr;
		raiseAlarmTest();
	}
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

