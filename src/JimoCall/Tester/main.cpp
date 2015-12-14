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
// 2014/12/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoHelloWorld.h"
#include "JimoCall/Tester/TestSuite.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
#include "SEInterfaces/DllMgrObj.h"
#include "SEInterfaces/ClusterMgrObj.h"
// #include "ClusterMgr/ClusterMgr.h"
#include "aosDLL/DllMgr.h"
#include "UtilData/ClusterNames.h"
#include "Jimo/JimoDummy.h"
#include "JimoCallServer/JimoCallServer.h"
  

extern void clear_sgDfmDocs();
extern void clear_sgClusterMgr();
 
int gAosShowNetTraffic = 0;

int 
main(int argc, char **argv)
{
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.1");

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


	OmnWait::getSelf()->start();
	OmnTimeDriver::getSelf()->start();
	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr());

	AosRundataPtr rdata = OmnApp::getRundata();
	// OmnScreen << "...........Start JimoCallServer........." << endl;
	// AosJimoCallServer jimocall_server(rdata.getPtr());

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("JimoCall", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosJimoCallTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();


//	OmnString value;
//	OmnString message = "JimoCall_HelloWorld";
//	bool rslt = Jimo::jimoCallSyncRead(rdata.getPtr(), message, value);
//	aos_assert_r(rslt, false);
//	OmnScreen << ", value : " << value << endl;



	cout << "\nFinished. " << testMgr->getStat().data() << endl;
	//testMgr = 0;
 
	cout << "Program exit normally!!!!!!!!!!!" << endl;

	//clear_sgDfmDocs();
	//AosDllMgrObj *dllmgr_obj = AosDllMgrObj::getDllMgr1();
	//AosDllMgr *dllmgr = dynamic_cast<AosDllMgr *>(dllmgr_obj);
	//dllmgr->clear_mJimoMap();
	//clear_sgClusterMgr();

	return 0;
} 

