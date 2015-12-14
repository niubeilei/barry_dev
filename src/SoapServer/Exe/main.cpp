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
// 06/03/2011: Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "SEUtil/DocTags.h"
#include "SoapServer/SoapServer.h"
#include "XmlUtil/XmlTag.h"
#include "util2/global_data.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int gAosLogLevel = 1;
#include <dirent.h>


int 
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

	AosSoapServer::getSelf()->config(OmnApp::getAppConfig());
	AosSoapServer::getSelf()->start();
	
	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 


