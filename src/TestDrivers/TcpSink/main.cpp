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
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServer.h"
#include "XmlParser/XmlItem.h"
  
#include "TestDrivers/TcpSink/TcpSink.h"
 

int 
main(int argc, char **argv)
{
	OmnIpAddr localAddr;
	int localPort = -1;
	int numPorts = 1;

cout << "++++++++++ 1" << endl;
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

cout << "++++++++++ 2" << endl;

	int index = 1;
	while (index < argc)
	{
cout << "++++++++++ 3" << endl;
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
	
		if (strcmp(argv[index], "-n") == 0)
		{
			numPorts = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

cout << "++++++++++ 4" << endl;
	if (localAddr == OmnIpAddr::eInvalidIpAddr || localPort <= 0 || numPorts < 1)
	{
		cout << "Command incorrect!" << endl;
		exit(1);
	}

	AosTcpSink theSinker("TcpSink", localAddr, localPort, numPorts);
	OmnString errmsg;
	if (!theSinker.connect(errmsg))
	{
		cout << "********* Failed to connect: " << errmsg << endl;
		exit(1);
	}

	cout << "++++++++++++++ Listening on: " << localAddr.toString() << ":" << localPort << endl;

	theSinker.startReading();
		
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

 
