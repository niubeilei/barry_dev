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
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "XmlParser/XmlItem.h"
#include "PacketCheck/PCTestsuite.h"

#include "TestDrivers/TcpTrafficGen/TrafficGenTester.h" 
#include <pthread.h>

#include "Util/File.h"

const int maxTrafficGen = 30;
void * priStatus(void *);
int number_gen = 1;
AosTcpTrafficGen *genPtr[maxTrafficGen];
int remotePort = -1;
OmnIpAddr localAddr;
OmnIpAddr remoteAddr;
int numPorts =1;

	int 
main(int argc, char **argv)
{
	int repeat = -1;
	int concurrentConns = -1;
	int contentLen = 1002;
	u32 sec_time = 30;
	int i;
	std::string configFileName;
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
			remoteAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-local") == 0)
		{
			localAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-p") == 0)
		{
			remotePort = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-r") == 0)
		{
			repeat = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-conn") == 0)
		{
			concurrentConns = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-configfile") == 0)
		{
			configFileName = argv[index+1];
			index += 2;
			continue;
		}


		index++;
	}

	if (repeat <= 0)
	{
		cout << "Repeat is incorrect (must be > 0)" << endl;
		exit(1);
	}

	if (remoteAddr == OmnIpAddr::eInvalidIpAddr 
			|| remotePort <= 0 || numPorts < 1)
	{
		cout << "Command incorrect!" << endl;
		exit(1);
	}

	if (localAddr == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Missing local address. Use -local <addr> to specify" << endl;
		exit(1);
	}

	
	if(configFileName.empty())
	{
		cout << "Config file name can not be empty" << endl;
		exit(1);	
	}
		
	AosPCTestsuite* pTestSuit = new AosPCTestsuite(configFileName);
	pTestSuit->init();
	
	for(i=0; i<number_gen; i++) 
	{
		genPtr[i] = new AosTcpTrafficGen(
				i,
				localAddr,
				remoteAddr, 
				remotePort + i,
				1,
				repeat, 
				concurrentConns,
				pTestSuit);
		genPtr[i]->start();
	}
	bool isFinished = false;
	u8 sumFinished = 0; //to summary finished generator
	u32 lastPrint=0;
	pthread_t requestpri;
//	int ret;
//	ret = pthread_create(&requestpri,NULL, priStatus,NULL);
//	if(ret!=0)
//	{
//		printf("Create pthread error!\n");
//		exit(1);
//	}

	while (!isFinished)
	{
		lastPrint++;
		OmnSleep(1);

		for (i=0; i<number_gen; i++)
		{
			genPtr[i]->checkConns();
			if (lastPrint >= sec_time)
			{
				lastPrint = 0;
				for(int j=0; j<number_gen; j++ )
				{
			//		genPtr[j]->printStatus();
				}
			}

			if (genPtr[i]->checkFinish())
			{
				sumFinished++;
			}
		}

		if(sumFinished < number_gen) // at least one generator online
		{
			sumFinished = 0;
			isFinished = false;
		}

		else
		{
			isFinished = true;
		}

	}//end while(isFinished)

	for (i=0; i<number_gen; i++)
	{
		delete genPtr[i];
	}	

	theApp.exitApp();
	return 0;
} 

void * priStatus(void *)
{
	while(1)                                                                            
	{                                                                                   
		OmnSleep(1);                                                                    
		char c;                                                                         
		cin >> c;                                                                       

		switch (c)                                                                      
		{                                                                               
			case '1':                                                                   
				for (int i=0; i<number_gen; i++)                                                
				{                                                                           
					genPtr[i]->printStatus();                                              
				}                                                                           
				break;                                                                      
		}                                                                               

	}     
}
