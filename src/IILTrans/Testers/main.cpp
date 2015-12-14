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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "IILTrans/Testers/TestSuite.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILTransServer/IILTransServer.h"
#include "TransServer/TransServer.h"
#include "TransServer/TransSvrConn.h"
#include "TransServer/TransRespFileMgr.h"
#include "TransUtil/TransStatusMgr.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/Tester/UtilTestSuite.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
#include "WordMgr/WordMgr1.h"
#include "IILClient/IILClient.h"
#include "IILMgr/IILMgr.h"
  

int gAosLogLevel = 0;
 
int 
main(int argc, char **argv)
{

	system("cd /AOS/Data/Data-Ken-IILTrans\n ./script");
	OmnApp theApp(argc, argv);

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
			gAosLogLevel = atoi(argv[index+1]);
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

	AosMemoryChecker::getSelf()->start();
	AosWordMgr1::getSelf();
	AosIILIDIdGen::getSelf();
	AosIILMgr::getSelf();
	AosIILClient::getSelf();
	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosIILTransTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

