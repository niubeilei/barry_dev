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
	OmnIpAddr localAddr;
	int localPort = -1;

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

	OmnTcpServer theServer(localAddr, localPort, 1, "test", OmnTcp::eTermByNewLine);
	OmnString errmsg;
	OmnRslt rslt = theServer.connect(errmsg);
	if (!rslt)
	{
		OmnAlarm << "Failed to connect: " << errmsg << enderr;
		theApp.exitApp();
		return 0;
	}

	while (1)
	{
		OmnTcpClientPtr client;
		OmnConnBuffPtr buff;

		/*
		// Read the first byte
		theServer.readFrom(buff, client);
		char *dd = buff->getData();
		int len = (((unsigned int)dd[0]) << 8);
		cout << "Length: " << len << endl;

		// Read the second byte
		buff = 0;
		theServer.readFrom(buff, client);
		dd = buff->getData();
		len += (unsigned int)dd[0];
		cout << "Length: " << len << endl;
		*/

		// Read the data
		buff = 0;
		theServer.readFrom(buff, client);
		int data_len = buff->getDataLength();
		int len = buff->getDataLength();
		char *dd = buff->getData();
		//if (data_len == len)
		//{
		//	OmnTrace << "Read data: " << dd << endl;
			for (int i=0; i<len; i++) cout << "Byte: " << (int)dd[i] << endl;
	//		char tmp[2];
	//		tmp[1] = (len >> 8);
	//		tmp[0] = len;
	//		client->writeToSock(tmp, 2);
	//		client->writeToSock(dd, len);
client->writeToSock(buff->getData(), buff->getDataLength());
		//}
	}

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

 
