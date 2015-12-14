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

#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"

#include "UdpBouncer.h"

void print_Usage()
{
	cout<<""<<std::endl<<std::endl;

	cout<<"Usage of UdpBouncer:"<<std::endl;
	cout<<"\tUdpTrafficGen -la [local addr] -lp [local port]"<<endl;
	cout<<""<<std::endl;
	cout<<"Example:"<<std::endl;
	cout<<"\t./UdpBouncer.exe -la 127.0.0.1 -lp 3000"<<endl;
	cout<<""<<std::endl;
}

int 
main(int argc, char **argv)
{
	OmnIpAddr localAddr;
	int localPort;

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
	
		index++;
	}

	if (localPort < 0)
	{
		cout << "Need to set the ports" << endl;
		print_Usage();
		theApp.exitApp();
		return 0;
	}

	if (!localAddr)
	{
		cout << "Need to set the addresses" << endl;
		print_Usage();
		theApp.exitApp();
		return 0;
	}

	AosUdpBouncer udpBouncer(localAddr,localPort);
	udpBouncer.start();

	while(1)
	{
		OmnSleep(1);
	}

/*	OmnUdp udp("test", localAddr, localPort);
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
*/
	theApp.exitApp();
	return 0;
} 
 
