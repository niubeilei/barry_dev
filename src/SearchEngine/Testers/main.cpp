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
#include "MySQL/Ptrs.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SearchEngine/WordMgr.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngine/IIL.h"
#include "SearchEngine/IILMgr.h"
#include "SearchEngine/Testers/TestSuite.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/Tester/UtilTestSuite.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlParser/XmlItem.h"
  
AosXmlTagPtr gAosAppConfig;

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew AosWordMgrSingleton());
		theApp.startSingleton(OmnNew AosDocServerSingleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

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

		index++;
	}

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosSearchEngineTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

