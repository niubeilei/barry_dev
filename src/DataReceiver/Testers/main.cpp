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
// Modification History:
// 06/15/2011: Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"

#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "DataScanner/Testers/TestSuite.h"
#include "DataScanner/NetFileServer.h"
#include "Timer/TimerMgr.h"
#include "TransClient/TransClient.h"
#include "TransServer/TransServer.h"
#include "Thread/ThreadShellMgr.h"
#include "SEInterfaces/NetworkMgrObj.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			
int gAosShowNetTraffic = 1;	
int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	int index = 1;
	OmnApp::setConfig("config_norm.txt");
	OmnApp::appStart(argc, argv);
	int tries = 0;

	while (index < argc)
	{
		if (strcmp(argv[index], "-tries") == 0)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		index++;
	}
	
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);

	OmnString workingdir = app_config->getAttrStr(AOSCONFIG_WORKINGDIR, "");
OmnScreen << "workingdir: " << workingdir << endl;
	if (workingdir != "" && chdir(workingdir.data()) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}

	AosNetworkMgrObj::config();
	AosXmlTagPtr cc = app_config->getFirstChild(AOSCONFIG_TRANS_CLIENT);
	if (cc)
	{
		AosTransClientPtr client = OmnNew AosTransClient(AosServiceId::ePublicServer, cc);
		client->recover();
		AosTransClientObj::setTransClient(client);
	}

	AosTransServer::createPublicTransServer();
	AosNetFileServer::getSelf();

	OmnThreadShellMgr::getSelf()->start();
	AosTimerMgr::getSelf();
	AosXmlTagPtr testers = app_config->getFirstChild("testers");
	if (!testers)
	{
		OmnAlarm << "Missing testers" << enderr;
		return -1;
	}

	OmnString testname = testers->getAttrStr("name");
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DataScannerTester", "Try", "Chen Ding");
	testMgr->setTries(1);
	testMgr->addSuite(AosDataScannerTestSuite::getSuite(testname));
	testMgr->start();

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 

