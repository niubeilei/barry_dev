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
#include "DocMgr/DocMgr.h"
#include "DocMgr/Ptrs.h"
#include "DocClient/DocClient.h"
#include "IILClient/IILClient.h" 
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SeLogSvr/PhyLogSvr.h" 
#include "SeLogSvr/Ptrs.h" 
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEModules/SeRundata.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageMgr/StorageMgr.h"
#include "Security/SecurityMgr.h"
#include "TransServer/TransSvrConn.h"
#include "Util/OmnNew.h"
#include "util2/global_data.h"
#include "WordMgr/WordMgr1.h"
#include "XmlUtil/XmlTag.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int gAosLogLevel = 1;
#include <dirent.h>


int 
//clientMain(int argc, char **argv)
main(int argc, char **argv)
{
	aos_global_data_init();

	// AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");

	/*int t=1;
	while(t)
	{
		t =1;
	}
	OmnApp::setConfig("config_client.txt");
	*/
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
	OmnApp::setConfig("config_norm.txt");
	OmnApp theApp(argc, argv);

	///////////////////////////////////////////////////////
	////
	////start TransServer
	////
	/////////////////////////////////////////////////////////
	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if(chdir(workingdir) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}
	
	try
	{
		AosDocMgrPtr docmgr = AosDocMgr1::getSelf();
		AosDocMgrObj::setDocMgr(docmgr);
		//AosWordMgr1::getSelf();
		AosIILClient::getSelf()->config(OmnApp::getAppConfig());
		AosIILClient::getSelf()->start();
		AosIILClientObj::setIILClient(AosIILClient::getSelf());
		AosDocClient::getSelf()->config(OmnApp::getAppConfig());
		AosDocClient::getSelf()->start();
		AosStorageMgr::getSelf()->config(OmnApp::getAppConfig());
		AosStorageMgr::getSelf()->start();
		AosSecurityMgr::getSelf()->config(OmnApp::getAppConfig());
		AosSecurityMgr::getSelf()->start();
	}
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}


	AosPhyLogSvrPtr logsvr = OmnNew AosPhyLogSvr(OmnApp::getAppConfig());
	if (!logsvr)
	{
		OmnAlarm << "Init log server failed!" << enderr;
		theApp.exitApp();
	}
	logsvr->start();

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 


