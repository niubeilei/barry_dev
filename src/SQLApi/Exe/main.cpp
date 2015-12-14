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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtil/DocFileMgr.h"
#include "SEClient/SEClient.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "util2/global_data.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool sgIsBigIILServer = false;
int gAosShowNetTraffic = 0;
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
static u32 sgSiteid = 100;

#include <dirent.h>

AosXmlTagPtr gAosAppConfig;


// extern bool AosDataReader_convertData();
extern bool AosConvertXmlDoc(OmnString &xml);
extern bool AosDataReader_sendReq(const OmnString &contents);

int 
main(int argc, char **argv)
{
	aos_global_data_init();
	
	OmnApp::appStart(argc, argv);             
	OmnApp::setVersion("0.10");
	OmnApp theApp(argc, argv);
	
	int index = 1;
	OmnString username;
	OmnString password;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-username") == 0)
		{
			username = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-password") == 0)
		{
			password = argv[index+1];
			index += 2;
			continue;
		}

		index++;
	}

	AosNetworkMgrObj::config();

	char command[10000];
	AosSqlAPI sqlapi;
	while (1)
	{
		cout << "Please enter your statement: " << endl;
		cin >> command;

		if (strcmp(command, "exit")) break;
		sqlapi.runCommand(command);
	}

	return 0;
} 

