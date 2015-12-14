////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
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
// 01/03/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestMgr.h"
          
#include "AosTester/AosTestSuite.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/Udp.h"
#include "UtilComm/TcpServer.h"
#include "XmlParser/XmlItem.h"

#include "FastFTP/Receiver/FastFtpReceiver.h"
  
#include <pthread.h>

#include "Util/File.h"

const int maxTrafficGen = 30;
void * priStatus(void *);
int number_gen = 1;

int 
main(int argc, char **argv)
{
	OmnIpAddr localAddr;
	int localPort;
	OmnString filename;

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

	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-la") == 0)
		{
			localAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-lp") == 0)
		{
			localPort = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-f") == 0)
		{
			filename = argv[index+1];
			index += 2;
			continue;
		}
	
		index++;
	}

	if (localPort < 0)
	{
		cout << "Need to set the ports" << endl;
		theApp.exitApp();
		return 0;
	}

	if (!localAddr)
	{
		cout << "Need to set the addresses" << endl;
		theApp.exitApp();
		return 0;
	}

	if (filename.length() <= 0)
	{
		cout << "Missing file name" << endl;
		theApp.exitApp();
		return 0;
	}

	AosFastFtpReceiver receiver(filename, localAddr, localPort);
	receiver.start();

	while (1)
	{
		OmnSleep(1);
	}

	cout << "Finished: " << endl;

	theApp.exitApp();
	return 0;
} 
 
