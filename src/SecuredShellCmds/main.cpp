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
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "aosUtil/Tracer.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SecuredShell/ReadConfig.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>



int 
main(int argc, char **argv)
{	
	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	/*
	while (index < argc)
	{
		if (strcmp(argv[index], "-cmd") == 0)
		{
			cmd = argv[index+1];
			cmd_req = 1;
			index += 2;
			continue;
		}
	}
	*/

	AosReadConfig theConfig("config.txt");

	cout << "Hello, World!" << endl;

	// theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

