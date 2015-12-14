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
// 7/18/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "aosUtil/Tracer.h"
#include "AppMgr/App.h"
#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/FuncModule.h"
#include "CliTorturer/CliCmdTorturer.h"
#include "CliTorturer/CliModuleTorturer.h"
#include "Debug/Debug.h"
#include "KernelInterface/KernelInterface.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestMgr.h"
#include "Tester/TestMgr.h"
#include "Torturer/ParmIncls.h"
#include "TorturerGen/TorturerGen.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/GenTable.h"
#include "Util/VarList.h"

#include "AutoTorturing/src/QoS/qos_aos_qos_enable_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_add_macrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_remove_macrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_clear_macrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_add_generalrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_remove_generalrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_clear_generalrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_retrieve_generalrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_set_bandwidth_control_status_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_retrieve_macrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_set_max_history_interval_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_add_portrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_remove_portrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_clear_portrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_retrieve_portrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_add_protocolrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_remove_protocolrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_clear_protocolrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_retrieve_protocolrule_Tester.h"
#include "AutoTorturing/src/QoS/qos_aos_qos_set_minimum_bandwidth_Tester.h"
#include "aosApi.h"
#include "AutoTorturing/src/QoS/InstGen_qos_genrule.h"
#include "AutoTorturing/src/QoS/InstGen_qos_macrule.h"
#include "AutoTorturing/src/QoS/InstGen_qos_portrule.h"
#include "AutoTorturing/src/QoS/InstGen_qos_protorule.h"


#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sqlite3.h>

extern bool api_torturer_special_case();

void printHelp()
{
	cout << "Usage: \n"
		<< "Torturer.exe [-l <logfilename>]: the log file name\n"
		<< "             -f <configFilename>: the configuration file name\n"
		<< "             -src <dirname>: The source code directory name\n"
		<< "             [-name <torturername>]: The torturer name (default: Torturer.exe)\n"
		<< "             [-n <numCommandsToRun>]: The number of commands to run\n"
		<< "             [-1 <torturingTime>]: The torturing time\n"
		<< "             [-m]: To print logs on screen\n" 
		<< "             [-repeat]: Repeat the testing (using the log.txt)\n" 
		<< "             [-flush]: To flush the log before calling functions\n"
		<< "             [-correct_only]: Will generate only correct calls\n"
		<< "             [-datadir <dir>]: The directory where configure files are stored\n"
		<< "             [-aoshome]: AOS home directory\n"
		<< "             [-test]: It is test only. It will not actually call the APIs\n"
		<< "             [-help]: To print this manual"<< endl;
}

static void registerApiFuncs();


int 
main(int argc, char **argv)
{	
	OmnString configFilename;
	OmnString logFilename;
	u32 cmdsToRun = 0;
	u32 torturingTime = 0;
	bool onScreen = false;
	bool repeat = false;
	bool toFlush = false;
	OmnString datadir;

	OmnApp theApp(argc, argv);
	try
	{
		// theApp.startSingleton(OmnNew OmnCliProcSingleton());
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

		if (strcmp(argv[idx], "-datadir") == 0)
		{
			datadir = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-l") == 0)
		{
			logFilename = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-aoshome") == 0)
		{
			AosCliTorturer::setAosHome(argv[idx+1]);
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-m") == 0)
		{
			onScreen = true;
			idx += 1;
			continue;
		}

		if (strcmp(argv[idx], "-repeat") == 0)
		{
			repeat = true;
		}
		
		if (strcmp(argv[idx], "-flush") == 0)
		{
			toFlush = true;
		}
		
		if (strcmp(argv[idx], "-test") == 0)
		{
			AosCliTorturer::setTestOnlyFlag(true);
		}

		if (strcmp(argv[idx], "-correct_only") == 0)
		{
			AosCliTorturer::setCorrectOnly(true);
		}

		idx++;
	}

	if (repeat)
	{
		// api_torturer_special_case();
		theApp.exitApp();
		return 0;
	}

	if (configFilename == "")
	{
		cout << "Missing configuration file name. \n";
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

	if (!AosCliTorturer::getTestOnlyFlag())
	{
		    
    aos_qos_enable(0);
    aos_qos_clear_macrule();
    aos_qos_clear_generalrule();
    aos_qos_set_bandwidth_control_status("off");
    aos_qos_clear_portrule();
    aos_qos_clear_protocolrule();
    

	}

    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("ApiTorturer", "Try", "Ding");
	testMgr->setCrtSuite(OmnNew OmnTestSuite("ApiTorturer", "ApiTorturer"));

	if (!testMgr->getCrtSuite())
	{
		OmnTrace << "Crt suite is null" << endl;
	}

	registerApiFuncs();

	AosCliTorturer tort(cmdsToRun, torturingTime, configFilename, 
		logFilename, logType, false, datadir);
	if (!tort.isGood())
	{
		OmnAlarm << "Failed to create the tortorer" << enderr;
		theApp.exitApp();
		return 0;
	}

	tort.setTestMgr(testMgr);
	tort.setFlushFlag(toFlush);

	tort.start();

    cout << "\nFinished: \n" 
		<< testMgr->getStat() << "\n\n"
		<< AosCliCmdTorturer::getStat() << endl;

	theApp.exitApp();
	return 0;
} 


void registerApiFuncs()
{
	AosClass_aos_qos_enable::registInst();
AosClass_aos_qos_add_macrule::registInst();
AosClass_aos_qos_remove_macrule::registInst();
AosClass_aos_qos_clear_macrule::registInst();
AosClass_aos_qos_add_generalrule::registInst();
AosClass_aos_qos_remove_generalrule::registInst();
AosClass_aos_qos_clear_generalrule::registInst();
AosClass_aos_qos_retrieve_generalrule::registInst();
AosClass_aos_qos_set_bandwidth_control_status::registInst();
AosClass_aos_qos_retrieve_macrule::registInst();
AosClass_aos_qos_set_max_history_interval::registInst();
AosClass_aos_qos_add_portrule::registInst();
AosClass_aos_qos_remove_portrule::registInst();
AosClass_aos_qos_clear_portrule::registInst();
AosClass_aos_qos_retrieve_portrule::registInst();
AosClass_aos_qos_add_protocolrule::registInst();
AosClass_aos_qos_remove_protocolrule::registInst();
AosClass_aos_qos_clear_protocolrule::registInst();
AosClass_aos_qos_retrieve_protocolrule::registInst();
AosClass_aos_qos_set_minimum_bandwidth::registInst();
    qos_genrule_InstGen::registerInstanceGen("qos_genrule");
    qos_macrule_InstGen::registerInstanceGen("qos_macrule");
    qos_portrule_InstGen::registerInstanceGen("qos_portrule");
    qos_protocolrule_InstGen::registerInstanceGen("qos_protorule");

}


