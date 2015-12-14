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
// 2013/10/29 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "Jimo/JimoDummy.h"
#include "Schema/SchemaDummy.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Schema/Jimos/Testers/SchemaTestSuite.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/MemoryChecker.h"
#include "Util/Tester/ValueRsltTester.h"
using namespace std;
  
 
int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	OmnAlarmMgr::config();

//	AosScheduleTime(0, 0);

	int index = 1;
	int tries = 0;
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
		
		index++;
	}
	
	AosSchemaDummy dummy_schema(0, 0);
	dummy_schema.getVersion();
	// OmnScreen << "Jimo version: " << jimo.getVersion() << endl;

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Schema Testers", "Try", "Jackie");
	testMgr->setTries(tries);
	testMgr->addSuite(AosSchemaTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

 
