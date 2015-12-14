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
#include "ZikiApps/PollDbChanges/PollDbChanges.h"
  
#include <stdlib.h>


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

	int idx = 0;
	OmnString tablename;
	int snappoint = -1;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-snap") == 0)
		{
			snappoint = atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-tablename") == 0)
		{
			tablename = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;
	}

	if (tablename == "")
	{
		OmnAlarm << "Missing tablename" << enderr;
		theApp.exitApp();
		return -1;
	}

	if (snappoint <= 0)
	{
		OmnAlarm << "Missing snappoint" << enderr;
		theApp.exitApp();
		return -1;
	}


	OmnTrace << "To process. Snamppoint: " << snappoint
		<< ". Tablename: " << tablename << endl;
	AosPollDbChanges dbpoll;
	int rslt = dbpoll.process(snappoint, tablename);
	OmnTrace << "Processed: " << rslt << endl;
	theApp.exitApp();
	return rslt;
} 
