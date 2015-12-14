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
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/CliCmdTorturer.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/CliModuleTorturer.h"
#include "Debug/Debug.h"
#include "KernelInterface/KernelInterface.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestMgr.h"
#include "Tester/TestMgr.h"
#include "TorturerGen/TorturerGen.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/GenTable.h"
#include "Util/VarList.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
//#include <sqlite3.h>

void printHelp()
{
	cout << "Usage: \n"
		<< "Torturer.exe \n"
		<< "    -f <configFilename>:  The configuration file name\n"
		<< "    -src <dir-name>:      Source file directory\n" 
		<< "    [-l <logfilename>]:   The log file name\n"
		<< "    [-n <numCmdsToRun>]:  The number of commands to run\n"
		<< "    [-1 <torturingTime>]: The torturing time\n"
		<< "    [-m]:                 To print logs on screen\n" 
		<< "    [-flush]:             Flush log before executing it\n"
		<< "    [-name name]:         Executable name (default: Torturer.exe)\n"
		<< "    [-help]:              To print this manual"<< endl;
}


int 
main(int argc, char **argv)
{	
	OmnString configFilename;
	OmnString logFilename;
	u32 cmdsToRun = 0;
	u32 torturingTime = 0;
	bool onScreen = false;
	bool flush = false;
	OmnString srcdir;
	OmnString objname = "Torturer.exe";
	OmnString datadir;

	OmnApp theApp(argc, argv);
	try
	{
		// theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
		theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
//		theApp.startSingleton(OmnNew OmnCliProcSingleton());

	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	int idx = 0;
	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printHelp();
			theApp.exitApp();
			return 0;
		}

		if (strcmp(argv[idx], "-n") == 0)
		{
			cmdsToRun = atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-t") == 0)
		{
			torturingTime = atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-f") == 0)
		{
			configFilename = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-l") == 0)
		{
			logFilename = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-m") == 0)
		{
			onScreen = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-flush") == 0)
		{
			flush = true;
		}

		if (strcmp(argv[idx], "-src") == 0)
		{
			srcdir = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-name") == 0)
		{
			objname = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-datadir") == 0)
		{
			datadir = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;

	}

	if (configFilename == "")
	{
		cout << "Missing configuration file name. \n";
		printHelp();
		theApp.exitApp();
		return -1;
	}

	if (srcdir == "")
	{
		cout << "Missing source code directory name. \n";
		printHelp();
		theApp.exitApp();
		return -1;
	}

	AosCliTorturerLogType logType;
	if (logFilename != "")
	{
		logType = eAosCliTorturerLogType_File;
	}
	else if (onScreen)
	{
		logType = eAosCliTorturerLogType_OnScreen;
	}
	else
	{
		logType = eAosCliTorturerLogType_NoLog;
	}

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("CliTorturer", "Try", "Ding");
	testMgr->setCrtSuite(OmnNew OmnTestSuite("CliTorturer", "CliTorturer"));

	if (!testMgr->getCrtSuite())
	{
		OmnTrace << "Crt suite is null" << endl;
	}

	AosCliTorturer tort(cmdsToRun, torturingTime, 
			configFilename, logFilename, logType, 
			srcdir, objname, true, datadir);
	if (!tort.isGood())
	{
		OmnAlarm << "Failed to create the tortorer" << enderr;
		theApp.exitApp();
		return 0;
	}

	if (true)
	{
		tort.genCode(true);
		tort.saveCommandStatus(configFilename);
		tort.saveModuleStatus(configFilename);
		theApp.exitApp();
		return 0;
	}

	tort.setTestMgr(testMgr);
	tort.setFlushFlag(flush);
	tort.start();
	
	// if (saveCommandStatus)
	// {
	// 	tort.saveCommandStatus(configFilename);
	// }

    cout << "\nFinished. \n" 
		<< AosCliCmdTorturer::getStat() << "\n\n"
		<< testMgr->getStat() << endl;

	theApp.exitApp();
	return 0;
} 

