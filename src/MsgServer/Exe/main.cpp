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
// 08/15/2010: Created by James
////////////////////////////////////////////////////////////////////////////
#include "AppMgr/App.h"
#include "MsgServer/MsgReqProc.h"
#include "MsgServer/AllMsgReqProcs.h"
#include "MsgServer/MsgServer.h"
#include "MsgServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SmartDoc/SmartDoc.h"
#include "SEModules/SeRundata.h"
#include "SEModules/Ptrs.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "util2/global_data.h"
#include <stdlib.h>
#include <list>
#include <signal.h>
#include <iostream>
using namespace std;

static bool needStop = false;

int gAosShowNetTraffic = 1;
bool sgIsBigIILServer = false;

void aosSignalHandler(int value)
{
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();
	OmnApp::appStart(argc, argv);

	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-log") == 0)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index++;
			continue;
		}

		if (strcmp(argv[index], "-config") == 0)
		{
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}
		index++;
	}

	OmnApp theApp(argc, argv);
	try
	{
		AosSengAdmin::getSelf();
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);
	AosNetworkMgrObj::config();
	AosTransModuleSvr::init(app_config->getFirstChild(AOSCONFIG_TRANS_SERVER));
	AosTransModuleSvr::recover();
	AosTransModuleSvr::start();

	
	///////////////////////////////////////////////////////
	//
	//start TransServer
	//
	///////////////////////////////////////////////////////
	AosMsgServer::getSelf()->start();

	///////////////////////////////////////////////////////
	//
	//start MsgServer
	//
	///////////////////////////////////////////////////////
	AosMsgReqProc::config(app_config);
	AosMsgReqProcPtr msgproc = OmnNew AosMsgReqProc();
	AosReqDistr msg_reqDistr(msgproc);
	if (!msg_reqDistr.config(app_config))
	{
		OmnAlarm << "Failed the configuration: " << app_config->toString() << enderr;
		theApp.exitApp();
		return -1;
	}
	msg_reqDistr.start();


	while (1)
	{
		OmnSleep(100000);
	}
	theApp.exitApp();
	return 0;
} 

