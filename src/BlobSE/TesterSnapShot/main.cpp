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
#include "BlobSE/TesterSnapShot/BlobSETester.h"
#include "alarm_c/alarm.h"
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Porting/Sleep.h"
#include "BlobSE/TesterSnapShot/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util1/Wait.h"
#include "Util1/TimeDriver.h"
#include "XmlUtil/XmlTag.h"
#include <getopt.h>
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/BitmapObj.h"
#include "NetworkMgr/NetworkMgr.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/ReplicMgrObj.h"
#include "ReplicMgr/ReplicMgr.h"

AosXmlTagPtr	gApp_config;
i64				gTestDuration = 0;
u32				gRandomSeed = 0;
int gAosShowNetTraffic = 1;
bool			gSanityCheck = true;

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
	int opt = 0;

	while((opt = getopt(argc, argv, "c:s:t:")) != -1)
	{
		switch (opt)
		{
		case 'c':
			gApp_config = OmnApp::readConfig(optarg);
			if(!gApp_config)
			{
				std::cout << "Can not read config file." << endl;
				usage(argv[0]);
				return -1;
			}
			break;
		case 's':
			gRandomSeed = atoi(optarg);
			break;
		case 't':
			gTestDuration = atoi(optarg);
			break;
		default:
			std::cout << "Invalid argument." << endl;
			usage(argv[0]);
			return -99;
			break;
		}
	}

	OmnApp theApp(argc, argv);
	AosBitmapObj::setObject(OmnNew AosBitmap());
	OmnAlarmMgr::config();
	OmnWait::getSelf()->start();
	OmnTimeDriver::getSelf()->start();
	OmnSleep(1);	//waiting for the time driver to start working

	OmnApp::setConfig(gApp_config);
	
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(gApp_config);
	
	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("BlobSE/TesterSnapShot", "BlobSE Test", "White");
	testMgr->addSuite(AosBlobSETestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
	OmnSleep(10);
	//int childpid = waitpid();
	
	theApp.exitApp();
	return 0;
} 
