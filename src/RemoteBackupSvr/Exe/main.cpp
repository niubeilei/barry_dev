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
// 02/15/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "Porting/Sleep.h"
#include "RemoteBackupSvr/RemoteBackupSvr.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "StorageMgr/StorageMgr.h"
#include "util2/global_data.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>

static bool AosExitApp();
void aosSignalHandler(int value)
{
	if (value == 14) AosExitApp();
}

bool AosExitApp()
{
	return true;
}

int gAosLogLevel = 1;
static bool setConfig(const char *fname);



int 
main(int argc, char **argv)
{
	int index = 1;
	aos_global_data_init();
	
	//AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);             
	OmnApp::setVersion("0.10");
	OmnApp::setFrontEndServer(false);

	OmnApp theApp(argc, argv);
	OmnString fname = "config_norm.txt";
	OmnString pattern, value;

	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			fname = argv[index+1];
			index = index + 2;
			//break;
			continue;
		}
		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
			// '-log xx'
			gAosLogLevel = atoi(argv[index+1]);
			index = index + 2;
			continue;
		}
		index++;
	}

	OmnApp::setConfig(fname.data());
	setConfig(fname.data());

	AosXmlTagPtr config = OmnApp::getAppConfig();
	if (!config)
	{
		OmnAlarm << "Missing configuration" << enderr;
		exit(-1);
	}

	OmnDataStoreMySQL::startMySQL(argc, argv);

	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if (workingdir != "" && chdir(workingdir.data()) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}
	//AosXmlDoc::staticInit(OmnApp::getAppConfig());
	
	// Configuring singleton classes
	//AosStorageMgr::getSelf()->config(OmnApp::getAppConfig());
	//AosSecurityMgr::getSelf()->config(config);
	//AosSessionMgr::getSelf()->config(config);
		
	// Start singleton classes
	//AosStorageMgr::getSelf()->start();
	//AosSecurityMgr::getSelf()->start();
	//AosSessionMgr::getSelf()->start();

	AosNetReqProcPtr server = OmnNew AosRemoteBackupSvr();
	AosRemoteBackupSvr::startStatic(server);

	index = 1;
	while(index < argc)
	{
		index++;
	}

	while(1)
	{
		OmnSleep(3);
	}
	theApp.exitApp();
	return 0;
} 


static bool setConfig(const char *fname)
{
	AosXmlTagPtr gAosAppConfig;
	if (!fname)
	{
		OmnAlarm << "Missing file name!" << enderr;
		exit(-1);
	}

	OmnFile f(fname, OmnFile::eReadOnly);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open the config file: " << fname << enderr;
		exit(-1);
	}

	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnAlarm << "Failed to read config: " << fname << enderr;
		exit(-1);
	}

	AosXmlParser parser;
	gAosAppConfig = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (!gAosAppConfig)
	{
		OmnAlarm << "Configuration file incorrect: " << fname << enderr;
		exit(-1);
	}

	gAosAppConfig->setAttr(AOSTAG_ALLOW_CRTSPUSR, "false");
	AosXmlDoc::staticInit(gAosAppConfig);
	
	return true;
}


