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
// 06/03/2011: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DocClient/DocClient.h"
#include "DocMgr/DocMgr.h"
#include "DocMgr/Ptrs.h"
#include "IILClient/IILClient.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Rundata/Rundata.h"
#include "TransServer/TransServer.h"
#include "TransServer/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "ShortMsgSvr/AllShortMsgProc.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/ShortMsgSvr.h"
#include "ShortMsgSvr/AllShortMsgProc.h"
#include "util2/global_data.h"
#include "TransServer/TransSvrConn.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>

bool AosExitSearchEngine();

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;      
int gAosShowNetTraffic = 1;   

bool sgIsBigIILServer = false;
bool gNeedBackup = false;

static bool needStop = false;

void aosSignalHandler(int value)
{                                    
	if (value == 14) needStop = true;
}
	
int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");


	OmnApp::setConfig("config_norm.txt");
	OmnApp theApp(argc, argv);
		
	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-log") == 0)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index++;
			continue;
		}

		index++;
	}	


	// Change the directory to woring directory.
	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if(chdir(workingdir.data()) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}
	

	// Init the module the short message server used.
	try
	{
		AosQueryRsltMgr::getSelf()->config(OmnApp::getAppConfig());
		AosDocMgrPtr docmgr = AosDocMgr1::getSelf();
		AosDocMgrObj::setDocMgr(docmgr);
		//AosWordMgr1::getSelf();
		AosIILClient::getSelf()->config(OmnApp::getAppConfig());
		AosIILClient::getSelf()->start();
		AosIILClientObj::setIILClient(AosIILClient::getSelf());
		AosDocClient::getSelf()->config(OmnApp::getAppConfig());
		AosDocClient::getSelf()->start();
	}
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	
	// Start the short message server.
	AosShortMsgSvr::getSelf()->config(OmnApp::getAppConfig());
	AosShortMsgSvr::getSelf()->start();

	while (1)
	{
		OmnSleep(10000);
		if (needStop)
		{
			AosExitSearchEngine();
			break;
		}
	}

	theApp.exitApp();
	return 0;
} 


bool AosExitSearchEngine()
{
	AosShortMsgSvr::getSelf()->closeGsmMgr();
	return true;
}
