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
#include "AppMgr/App.h"
#include "CliComm/CliCommSvr.h"
#include "CliComm/CliRequest.h"
#include "CliComm/CliSession.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/Cli.h"
#include "SingletonClass/SingletonMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"

  
int 
main(int argc, char **argv)
{	
	cout << "To start: " << endl;

	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	OmnKernelApi::init();

	// 
	// Load the configuration
	//
	AosCli cli("");
	OmnString rslt;
	if (!cli.loadConfig(rslt))
	{
		OmnAlarm << "Failed to load configuration: "
			<< rslt << enderr;
	}

	//  
	// This is the application part
	//
	OmnString errmsg;
	AosCliCommSvr theServer("CliCommSvr", OmnIpAddr("127.0.0.1"), 5500, 1);
	if (!theServer.connect(errmsg))
	{
		OmnAlarm << "Failed to connect!" << enderr;
		exit(-1);
	}

	theServer.startReading();

	theApp.appLoop();
	theApp.exitApp();
	return 0;              
} 
 

