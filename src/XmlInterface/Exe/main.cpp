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
#include "XmlInterface/XmlProc.h"


#include <stdlib.h>
#include <dirent.h>

int aos_getFiles();

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
	int idx = 0;
	OmnString filename;
	int seed = 0;
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

	if (filename == "")
	{
		OmnAlarm << "Missing filename" << enderr;
		theApp.exitApp();
		return -1;
	}

	srand(seed);

	AosXmlProc theproc;

	OmnTrace << "To process: " << filename << endl;
	int rc = theproc.process(filename);
	OmnTrace << "Processed!" << endl;

	aos_getFiles();
	//sleep(1);
	theApp.exitApp();
	return rc;
} 

int aos_getFiles()
{
	struct dirent **namelist;
	int n;
	n = scandir(".", &namelist, 0, alphasort);
	if (n < 0)
		OmnAlarm << "Failed to scan" << enderr;
	else
	{
		while(n--)
		{
			printf("%s\n", namelist[n]->d_name);
			free(namelist[n]);
		}
		free(namelist);
	}

	return 0;
}

