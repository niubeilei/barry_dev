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
  
#include "TestDrivers/TcpTrafficGen/TrafficGenTester.h" 
#include <pthread.h>

#include "Util/File.h"
#include "TestDrivers/TcpTrafficGen/GenThread.h"

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
	int numThread = 1;
	AosSendBlockType sendBlockType = eAosSendBlockType_Fixed;
	u32 sendBlockSize = 10000;
	int expectedBytes = -1;
//	const int maxTcpClients = 2000;
//	OmnTcpClient* tcpClients[maxTcpClients];
	bool switchPmi = 0; // 1: send 'http header'; 0: send 'c'
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
	OmnGenThread http_get[100];

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

		if (strcmp(argv[index], "-len") == 0)
		{
			contentLen  = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-sec") == 0)
        {
            sec_time = atoi(argv[index+1]);
			index += 2;
            continue;
        }

		if (strcmp(argv[index], "-numgen") == 0)
        {
            number_gen = atoi(argv[index+1]);
            index += 2;
			if (number_gen > maxTrafficGen)
			{
				cout << "Too many traffic generators" << endl;
				exit(0);
			}
            continue;
        }

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

		if (strcmp(argv[index], "-blocksize") == 0)
        {
            sendBlockSize = atoi(argv[index+1]);
            index += 2;
			continue;
		}

		if (strcmp(argv[index], "-exp") == 0)
        {
            expectedBytes = atoi(argv[index+1]);
            index += 2;
			continue;
		}

		if (strcmp(argv[index], "-pmi") == 0)
		{
			switchPmi = 1;
			index ++;
			continue;
		}

		if (strcmp(argv[index], "-numthread") == 0)
		{
			numThread = atoi(argv[index+1]);
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

	if (expectedBytes == 0)
	{
		OmnString contents;
		int lenStr;
		if (switchPmi == 1)
// add by xiaoqing 2005-12-12
		{
/*			OmnFile file("gethttp", OmnFile::eReadOnly);

            if(!file.isGood()&&file.openFile(OmnFile::eReadOnly))
            {
                cout << "Failed to read the file:"
                     << "gethttp" << endl;
                goto EXIT;
            }
            if(!file.readAll(contents))
            {
                cout << "Failed to read the file:"
                     << "gethttp" << endl;
                goto EXIT;
            }
            lenStr = strlen(contents.data());
            if(!file.closeFile())
            {
                cout << "Failed to close the file:"
                     << "gethttp" << endl;
                goto EXIT;
            }*/
			for (int inc = 0; inc < numThread; inc++)
			{
				http_get[inc].start();
				//http_get[inc].stop();
			}
		}

		else if (switchPmi == 0)
		{
			lenStr = contentLen;
			contents.setSize(contentLen+100);
			contents.set('c',contentLen);
		}
/*
		const char *data = contents.data();
		u32 mStartSec;
		u32 mStartSec1;
		u32 mTime;
		// 
		// This is to generate traffic only
		//
		OmnString err;
		u64 bytesTotal=0;
		mStartSec1 = OmnTime::getSecTick();
		for (int r=0; r<repeat; r++)
		{
			cout << "\n" << "======================================" << endl;
			cout << "Repeat: " << repeat << " No." << r+1 <<endl;

			mStartSec = OmnTime::getSecTick();
			cout << "First Time is : mStartSec = " << mStartSec << endl;
			for (i=0; i<concurrentConns; i++)
			{
				tcpClients[i] = OmnNew OmnTcpClient("tcp", remoteAddr, remotePort, numPorts, OmnTcp::eNoLengthIndicator);
				if (!tcpClients[i])
				{
					OmnAlarm << "Failed to create tcpClient" << err << enderr;
					return -1;
				}
				if (!tcpClients[i]->connect(err))
				{
					OmnAlarm << "Failed to connect: " << err << enderr;
				}
			}

			int bytesSent = 0;
			while (bytesSent < lenStr)
			{
				//u32 sendLen = contentLen - bytesSent;
				u32 sendLen = lenStr - bytesSent;
				if (sendLen > sendBlockSize) sendLen = sendBlockSize;

				for (i=0; i<concurrentConns; i++)
				{
					if (!tcpClients[i]->writeTo(data + bytesSent, sendLen))
					{
						OmnAlarm << "Failed to send: " << enderr;
					}
				}

				bytesSent += sendLen;
			}
			for (i=0; i<concurrentConns; i++)
            {
				if (tcpClients[i])
				{
					tcpClients[i]->closeConn();
					OmnDelete(tcpClients[i]);
				}
            }
			bytesTotal += bytesSent;
		}
		bytesTotal *= concurrentConns;
		cout << "time "<< OmnTime::getSecTick()<< "\n" << endl;
		cout << "mStartSec "<< mStartSec1 
			<< "\ntotal "<< bytesTotal<<"\n" << endl;
		if((mTime = (OmnTime::getSecTick() - mStartSec1)) > 0)
		{
			cout << "Rate: " 
				<< (bytesTotal/(OmnTime::getSecTick() - mStartSec1)) 
				<< "\n" << endl;
		}
*/
OmnSleep(60);
	}
	else 
	{
		OmnString strv;
		if (switchPmi == 1)
		{	
			OmnFile file("gethttp", OmnFile::eReadOnly);
			if(!file.isGood()&&file.openFile(OmnFile::eReadOnly))
			{
				cout << "Failed to read the file:"
					 << "gethttp" << endl;
				goto EXIT;
			}
			if(!file.readAll(strv))
			{
				cout << "Failed to read the file:"
					 << "gethttp" << endl;
				goto EXIT;
			}

			if(!file.closeFile())
			{
				cout << "Failed to close the file:"
					 << "gethttp" << endl;
				goto EXIT;
			}			

			int lenStr = strlen(strv.data());

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
									strv.data(),
									lenStr,
									sendBlockType,
									sendBlockSize, 
									lenStr);
				genPtr[i]->start();
			}
			
		}
		if (switchPmi == 0)
		{
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
									'c',
									contentLen,
									sendBlockType,
									sendBlockSize, 
									contentLen);
				genPtr[i]->start();
			}
		}
		bool isFinished = false;
		u8 sumFinished = 0; //to summary finished generator
		u32 lastPrint=0;
		pthread_t requestpri;
		int ret;
		ret = pthread_create(&requestpri,NULL, priStatus,NULL);
		if(ret!=0)
		{
			printf("Create pthread error!\n");
			exit(1);
		}
         
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
 			      		genPtr[j]->printStatus();
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
	}
EXIT:	
	char c = '1';
	while (c != '0')
	{
		cout << "Enter the value: " << endl;
		int value;
		cin >> value;

		int vv = 4096;
		while (vv < value)
		{
			char * buffer = (char *)malloc(4096);
			if (!buffer)
			{
				cout << "***** Failed to allocate" << endl;
				break;
			}
			memset(buffer, 9, 4096);
			vv++;
		}

		cout << "Enter the command: " << endl;
		cin >> c;
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
