////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
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
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "TcpLoop/TcpLoopGroup.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "XmlParser/XmlItem.h"
  
#include "TestDrivers/TcpTrafficGen/TrafficGenTester.h" 
#include <pthread.h>

#include "Util/File.h"
#include "TestDrivers/TcpTrafficGen/GenThread.h"

static void printUsage()
{
	cout << "Usage: \n" 
		 << "    TcpLoop.exe \n"
		 << "        -remoteaddr <remote_addr>\n"
		 << "        -remotemask <remote_mask>\n"
		 << "        -serveraddr <server_addr>\n"
		 << "        -servermask <server_mask>\n"
		 << "        -remoteport <remote_port>\n"
		 << "        -localaddr  <local_addr> \n"
		 << "        -localmask  <local_mask>\n"
		 << "        -localport  <local_port> \n"
		 << "        -repeat     <number> (set the number of repeats. Default: 1)\n"
		 << "        -conns      <number> (set the number of connections. Default: 1)\n"
		 << "        -blocksize  <number> (set sending block size [1, 65535]. Default: 1000)\n"
		 << "        -sendsize   <number> (bytes a connection will send (-1 for unlimited)\n"
		 << "        -sendwait   <number> (set seconds to wait between two sends. Default: 0.1 sec)\n"
		 << "        -nobounce   (set the server not bouncing. Optional. Default: Bouncing)\n"
		 << "        -reportinterval <number> (Default: 5 seconds)\n"
		 << endl;
}

int 
main(int argc, char **argv)
{
	int repeat = 1;
	int numConns = 1;
	u32 sendBlockSize = 1000;
	OmnIpAddr localAddr, remoteAddr, localMask, remoteMask, serverAddr, serverMask;
	u16 localPort, remotePort;
	u32 sendWaitSec = 0;
	u32 sendWaitUsec = 10000;
	OmnTcpLoop::BounceMethod bounceMethod = OmnTcpLoop::eSimpleBounce;
	int reportInterval = 5;
	int sendSize = -1;

	OmnRandom::init();
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
		if (strcmp(argv[index], "-remoteaddr") == 0)
		{
			remoteAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-serveraddr") == 0)
		{
			serverAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-servermask") == 0)
		{
			serverMask = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-remotemask") == 0)
		{
			remoteMask = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-help") == 0)
		{
			printUsage();
			theApp.exitApp();
			return 0;
		}

		if (strcmp(argv[index], "-localaddr") == 0)
		{
			localAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-localmask") == 0)
		{
			localMask = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-nobounce") == 0)
		{
			bounceMethod = OmnTcpLoop::eDrop;
			index += 1;
			continue;
		}

		if (strcmp(argv[index], "-remoteport") == 0)
		{
			remotePort = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-sendsize") == 0)
		{
			sendSize = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-localport") == 0)
		{
			localPort = atoi(argv[index+1]);
			index += 2;
			continue;
		}
	
		if (strcmp(argv[index], "-reportinterval") == 0)
		{
			reportInterval = atoi(argv[index+1]);
			index += 2;
			continue;
		}
	
		if (strcmp(argv[index], "-repeat") == 0)
		{
			repeat = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-conns") == 0)
		{
			numConns = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		/*
		if (strcmp(argv[index], "-blocktype") == 0)
        {
			if (strcmp(argv[index+1], "random") == 0)
			{
				sendBlockType = eAosSendBlockType_Random;
			}
			else if (strcmp(argv[index+1], "fixed") == 0)
			{
				sendBlockType = eAosSendBlockType_Fixed;
			}

            index += 2;
            continue;
		}
		*/

		if (strcmp(argv[index], "-blocksize") == 0)
        {
            sendBlockSize = atoi(argv[index+1]);
            index += 2;
			continue;
		}

		if (strcmp(argv[index], "-sendwait") == 0)
		{
			float v = atof(argv[index+1]);
			sendWaitSec = (u32)v;
			sendWaitUsec = ((u32)(v * 1000000)) % 1000000;
			index += 2;
			continue;
		}

		index++;
	}

	if (repeat <= 0)
	{
		cout << "Repeat is incorrect (must be > 0)" << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (serverAddr == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Server address incorrect: " << serverAddr.toString() << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (remoteAddr == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Remote address incorrect: " << remoteAddr.toString() << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (!OmnIpAddr::isValidMask(remoteMask))
	{
		cout << "Remote mask incorrect: " << remoteMask.toString() << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (!OmnIpAddr::isValidMask(serverMask))
	{
		cout << "Server mask incorrect: " << serverMask.toString() << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (!OmnIpAddr::isValidMask(localMask))
	{
		cout << "Local mask incorrect: " << localMask.toString() << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (localAddr == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Local address incorrect: " << localAddr.toString() << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	if (sendSize == 0)
	{
		cout << "Send Size is invalid (0)" << endl;
		theApp.exitApp();
		printUsage();
		return 0;
	}

	OmnTcpLoopGroup loop(
		remoteAddr, remoteMask, 
		localAddr, localMask, 
		serverAddr, serverMask,
		numConns,
		bounceMethod, 
		sendBlockSize,
		sendSize,
		sendWaitSec,
		sendWaitUsec);

	OmnString errmsg;
	if (!loop.connect(errmsg))
	{
		OmnAlarm << "Failed to connect: " << errmsg << enderr;
		goto failed;
	}

	if (!loop.start())
	{
		OmnAlarm << "Failed to start" << enderr;
		goto failed;
	}

	while (1)
	{
		int sec = OmnTime::getSecTick();
		OmnSleep(reportInterval);
		int sec1 = OmnTime::getSecTick();
		int delta = sec1 - sec;
		if (delta == 0)
		{
			cout << "Received: " << loop.getTotalReceived() << endl;
		}
		else
		{
			cout << "Received: " << loop.getTotalReceived() << ". Rate: " 
			<< loop.getLastTotal() / delta << endl;
		}
		loop.resetLastTotal();
	}

	theApp.appLoop();
	return 0;

failed:
	theApp.exitApp();
	return 0;
} 
 
