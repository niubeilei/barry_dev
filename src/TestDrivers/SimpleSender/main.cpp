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
          
#include "alarm_c/alarm.h"
#include "AosTester/AosTestSuite.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpBouncer.h"
#include "XmlParser/XmlItem.h"
  
#include "TestDrivers/TcpBouncer/TcpBouncer.h"
#include "TestDrivers/TcpBouncer/Ptrs.h"
 

int 
main(int argc, char **argv)
{
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
	OmnIpAddr localAddr;
	int localPort;
	while (index < argc)
	{
		if (strcmp(argv[index], "-a") == 0)
		{
			localAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-p") == 0)
		{
			localPort = atoi(argv[index+1]);
			index += 2;
			continue;
		}
	
		index++;
	}

	while (1)
	{
		OmnTcpClientPtr client = new OmnTcpClient("test", localAddr, localPort, 1);
		OmnConnBuffPtr buff;

		OmnString errmsg;
		if (!client->connect(errmsg))
		{
			OmnAlarm << "Failed to connect" << enderr;
			return -1;
		}

		char len[2];
		len[0] = 0;
		len[1] = 5;
		client->writeToSock(len, 2);
		client->writeToSock("Linux", 5);

		bool timeout, broken;
		if (!client->readFrom(buff, 5, timeout, broken, false))
		{
			OmnAlarm << "Failed to read" << enderr;
			return false;
		}
		break;
	}

	theApp.exitApp();
	return 0;
} 

 
