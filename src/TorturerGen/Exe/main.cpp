////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 02/21/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "aosUtil/Tracer.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "TorturerGen/TorturerGen.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sqlite3.h>

int 
main(int argc, char **argv)
{	
	OmnString configFilename;
	bool force = false;

	OmnApp theApp(argc, argv);
	try
	{
		// theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " 
			<< e.toString() << endl;
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	int idx = 0;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-def") == 0)
		{
			configFilename = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-force") == 0)
		{
			force = true;
			idx ++;
			continue;
		}

		idx++;

	}

	if (configFilename == "")
	{
		cout << "Missing configuration file name. \n"
			<< "Usage: TorturerGen.exe -def configfilename" << endl;
		theApp.exitApp();
		return -1;
	}

	AosTorturerGen gen(configFilename, force);
	gen.genCode();

	theApp.exitApp();
	return 0;
} 

