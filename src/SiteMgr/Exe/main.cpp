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
// 12/17/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "MySQL/DataStoreMySQL.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "TorturerWrappers/ObjectWrapper.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "XmlInterface/Server/WebReqProc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/DbConvert.h"
#include "XmlInterface/Server/CmdLine.h"


#include <stdlib.h>


const OmnString sgConfig =
    "<config local_addr=\"loopback\""
   		"local_port=\"5565-5565\""
    	"service_rcd=\"true\""
	 	"service_name=\"SiteMgr\""
	  	"support_actions=\"true\""
	   	"req_delinerator=\"first_word_high\""
	    "max_proc_time=\"10000\""
		"max_conns=\"400\">"
	"</config>";

#include <dirent.h>

int 
main(int argc, char **argv)
{
	aos_global_data_init();
	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", "chen0318", 
		 		"mydb", 3306, "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosWSProcCmdLine(argc, argv);

	AosSiteReqProcPtr proc = OmnNew AosSiteReqProc();
	AosReqDistr reqDistr(proc);
	if (!reqDistr.config(sgConfig))
	{
		OmnAlarm << "Failed the configuration: " << sgConfig << enderr;
		theApp.exitApp();
		return -1;
	}
	reqDistr.start();

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 
