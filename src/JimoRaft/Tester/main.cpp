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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
#include "AppMgr/App.h"
#include "Alarm/AlarmMgr.h"
#include "Accesses/AllAccesses.h"  
#include "Accesses/Access.h"       
#include "aosDLL/DllMgr.h"
#include "NetFile/NetFileMgr.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"
#include "SEInterfaces/ReplicMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/CubeMapObj.h"
#include "SEInterfaces/DllMgrObj.h"
#include "SEInterfaces/IpcCltObj.h" 
#include "SEModules/SiteMgr.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageMgr/VirtualFileSysMgr.h"
#include "SvrProxyMgr/IpcClt.h"
#include "TransClient/TransClient.h"    
#include "TransServer/CubeTransSvrMgr.h"
#include "DocClient/DocClient.h"
#include "JimoCallServer/JimoCallServer.h"
#include "JimoRaft/Tester/RaftTester.h"
#include "Porting/Sleep.h"
#include "JimoRaft/Tester/JimoRaftTestSuite.h"
#include "Tester/TestPkg.h"
#include "Util1/TimeDriver.h"
#include "Util1/Wait.h"
#include "XmlUtil/XmlTag.h"
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/BitmapObj.h"
#include <stdlib.h>
#include <getopt.h>

AosXmlTagPtr	gAppConfig;
i64				gTestDuration = 0;
u32				gRandomSeed = 0;
int gAosShowNetTraffic = 0; 
bool gSanityCheck = true;

void usage(char* p)
{
	printf("Usage: %s\n", p);
	printf("\t-c config xml file\n");
	printf("\t-r random seed, for triggering bugs\n");
	printf("\t-t test duration, -1 for infinite, 0 for batch test\n");
}


int 
main(int argc, char **argv)
{
	//get command line parameters
	AosXmlTagPtr ggAppConfig = 0;
	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-c") == 0)
		{
			// '-config fname'
			gAppConfig = OmnApp::readConfig(argv[index+1]);
			aos_assert_r(gAppConfig, false);
			OmnApp::setConfig(gAppConfig);
			index += 2;
			continue;
		}
		
		index++;
	}

	if (!gAppConfig)
	{
		cout << "Usage: $./rafttester.exe -c <config_file>" << endl;
		return 0;
	}

	//init the application and relevant modules
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.10");
	OmnApp theApp(argc, argv);

	//init members needed by raft micro service
	AosRundataPtr rdata = OmnApp::getRundata();
	AosAccess::init(rdata);
	rdata->setSiteid(100);


	AosBitmapObj::setObject(OmnNew AosBitmap());
	OmnAlarmMgr::config();
	OmnWait::getSelf()->start();

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Raft/Tester", "JimoRaft Test", "Phil");
	testMgr->addSuite(AosJimoRaftTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
	theApp.exitApp();
	return 0;
} 

