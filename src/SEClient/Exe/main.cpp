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
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "MySQL/DataStoreMySQL.h"
#include "SEClient/SqlClient.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"


#include <stdlib.h>
#include <dirent.h>

static OmnString sgRemoteAddr="127.0.0.1";
static int sgRemotePort = 5565;


int 
main(int argc, char **argv)
{
	aos_global_data_init();


	int index = 1;
	OmnString passwd;
	int port = -1;
	OmnString siteid;
	OmnString username;
	OmnString appname;
	while (index < argc)
	{
		if (strcmp(argv[index], "-passwd") == 0 && index < argc-1)
		{
			passwd = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-appname") == 0 && index < argc-1)
		{
			appname = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-siteid") == 0 && index < argc-1)
		{
			siteid = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-uname") == 0 && index < argc-1)
		{
			username = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dbport") == 0 && index < argc-1)
		{
			port = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rtport") == 0 && index < argc-1)
		{
			int pp = atoi(argv[index+1]);
			index += 2;

			if (pp > 0) sgRemotePort = pp;
			continue;
		}

		if (strcmp(argv[index], "-rtaddr") == 0 && index < argc-1)
		{
			sgRemoteAddr = argv[index+1];
			index += 2;
			continue;
		}

		index++;
	}

	if (passwd == "")
	{
		cout << "********** Missing pasword!" << endl;
		return -1;
	}

	if (port == -1)
	{
		cout << "********** Missing port!" << endl;
		return -1;
	}

	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", passwd, 
		 		"mydb", port, "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	char cmd[1000];
	AosSqlClient client(OmnIpAddr(sgRemoteAddr), 
			sgRemotePort, siteid, appname, username);
	while (1)
	{
		cout << "se> ";
		cin.getline(cmd, 1000);

		if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit")== 0) exit(0);

		OmnString resp;
		OmnString errmsg;
		OmnScreen << cmd << endl;
		if (!client.procRequest(siteid, appname, username, cmd, resp, errmsg))
		{
			cout << "***** Failed: " << errmsg << endl;
		}
		else
		{
			cout << resp << endl;
		}
	}

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 
