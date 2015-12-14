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
// 04/17/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "MySQL/DataStoreMySQL.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "Proggie/ReqDistr/ReqDistr.h"


#include <stdlib.h>

const OmnString sgConfig = 
	"<config local_addr=\"loopback\""
		"local_port=\"5555-5556\""
		"service_rcd=\"true\""
		"service_name=\"ReqDistrTest\""
		"support_actions=\"true\""
		"req_delinerator=\"first_word_high\""
		"max_proc_time=\"10000\""
		"max_conns=\"400\">"
	"</config>";

int 
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", "chen0318", "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosVpdParser::init();
	/*
	OmnString filename;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-f") == 0)
		{
			filename = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-seed") == 0)
		{
			seed = atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		idx++;
	}
	*/

	int seed = 0;
	srand(seed);
	AosReqDistr req_distr;
	if (!req_distr.config(sgConfig))
	{
		OmnAlarm << "Failed the configuration: " << sgConfig << enderr;
		theApp.exitApp();
		return -1;
	}
	req_distr.start();

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 
