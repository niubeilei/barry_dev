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
#include "Proggie/ReqDistr/Samples/Processor.h"


#include <stdlib.h>

bool sample1();

static AosReqDistrPtr sgDistributor;

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

	if (!sample1())
	{
		theApp.exitApp();
		return -1;
	}

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 


bool sample1()
{
	// This sample creates an instance of AosMyProcessor, which is a subclass
	// of AosNetReqProc. The processor is configured through the following 
	// XML object. When running, the application will listen to port 5555. 
	// If 5555 is busy, it will listen to 5556. If a connection request is
	// detected, it creates the connection. It can create multiple connections.
	// For each connection, it listens on the inputs. If there are something
	// in the connection, it reads the data and calls the processor to 
	// process the request. 
	static OmnString lsConfig = 
		"<config local_addr=\"loopback\""
			"local_port=\"5555-5556\""
			"service_rcd=\"true\""
			"service_name=\"ReqDistrTest\""
			"support_actions=\"true\""
			"req_delinerator=\"first_word_high\""
			"max_proc_time=\"10000\""
			"max_conns=\"400\">"
		"</config>";

	AosNetReqProcPtr processor = OmnNew AosMyProcessor();
	sgDistributor = new AosReqDistr(processor);
	if (!sgDistributor->config(lsConfig))
	{
		OmnAlarm << "Failed the configuration: " << lsConfig << enderr;
		return false;
	}
	sgDistributor->start();
	return true;
}

