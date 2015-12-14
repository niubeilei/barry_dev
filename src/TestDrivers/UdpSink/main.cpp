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
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "XmlParser/XmlItem.h"
  
#include "TestDrivers/TcpTrafficGen/TrafficGenTester.h" 
#include <pthread.h>

#include "Util/File.h"

const int maxTrafficGen = 30;
void * priStatus(void *);
int number_gen = 1;
AosTcpTrafficGen *genPtr[maxTrafficGen];

int 
main(int argc, char **argv)
{
	OmnIpAddr remoteAddr, localAddr;
	int remotePort = -1, localPort;

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
		if (strcmp(argv[index], "-ra") == 0)
		{
			remoteAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rp") == 0)
		{
			remotePort = atoi(argv[index+1]);
			index += 2;
			continue;
		}

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

	OmnUdp udp("test", localAddr, localPort);
	OmnString errmsg;
	if (!udp.connect(errmsg))
	{
		cout << "************ Failed to connect: " << errmsg << endl;
		theApp.exitApp();
		return 0;
	}

	OmnConnBuffPtr buff;
	bool isTimeout;

	while (1)
	{
		udp.readFrom(buff, -1, 0, isTimeout);
		cout << "read contents. Sender: " 
			<< (buff->getRemoteAddr()).toString() << ":" << buff->getRemotePort()
			<< ". Receiver: " 
			<< (buff->getLocalAddr()).toString() << ":" << buff->getLocalPort()
			<< "\nContents: \n"
			<< buff->getBuffer() << endl;
	}

	theApp.exitApp();
	return 0;
} 
 
