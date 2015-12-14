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
// 05/18/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "ImageServer/ImageReqProc.h"
#include "ImageServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/MemMgr.h"
#include "util2/global_data.h"


#include <stdlib.h>

int gAosLogLevel = 1;


static OmnString sgConfig =
    "<config "
		"dirname=\"/home/AOS/Data\""
		"local_addr=\"loopback\""
		"service_rcd=\"true\""
		"remote_addr=\"127.0.0.1\""
		"remote_port=\"6666\""
		"service_name=\"ReqDistrTest\""
		"support_actions=\"true\""
		"req_delinerator=\"first_four_high\""
		"max_proc_time=\"10000\""
		"logfilename=\"zykielog\""
		"max_conns=\"400\""
		"local_port=\"6666-6666\"/>";


AosXmlTagPtr gAosAppConfig;


int 
main(int argc, char **argv)
{
	aos_global_data_init();

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(sgConfig, "");
	gAosAppConfig = root->getFirstChild();

	int index = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-log") == 0)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index++;
			continue;
		}

		index++;
	}

	OmnApp theApp(argc, argv);
	theApp.startSingleton(OmnNew AosSengAdminSingleton());
	AosSengAdmin::getSelf()->start(gAosAppConfig);

	// AosWSProcCmdLine(argc, argv);

	AosImageReqProc::config(gAosAppConfig);
	AosImageReqProcPtr proc = OmnNew AosImageReqProc();
	AosReqDistr reqDistr(proc);
	if (!reqDistr.config(sgConfig))
	{
		OmnAlarm << "Failed the configuration: " << sgConfig << enderr;
		theApp.exitApp();
		return -1;
	}
	reqDistr.start();

	while (1)
	{
		char contents[100];
		cin >> contents;
		if (strcmp(contents, "exit") == 0)
		{
			OmnScreen << "To stop server!" << endl;

			OmnSleep(3);
			OmnScreen << "Exiting!" << endl;
			break;
		}
	}

	// theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

