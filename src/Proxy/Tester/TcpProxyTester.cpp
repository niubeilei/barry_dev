////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpProxyTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Proxy/Tester/TcpProxyTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Porting/Sleep.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"

#include "UtilComm/TcpTrafficGenClient.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/Tcp.h"

#include "UtilComm/TcpClient.h"
//#include "UnitTest/BouncerCallback.h"
//#include "UnitTest/TrafficGenCallback.h"
//#include "UnitTest/OcspCrlStatus.h"
//#include "UnitTest/Ptrs.h"

#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpBouncer.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpClient.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"

//add the syn flood attack
#include "AttackProxy.h"

int AosTcpProxyTester::maxConns = 100;
//int AosTcpProxyTester::sendBlockSize = 1000;
int AosTcpProxyTester::sendBlockSize = 6000; //500 * 1024 * 1024;

//OmnString strSend = "okokok";
//int AosTcpProxyTester::mContentLen = strlen(strSend);
AosSendBlockType sendBlockType = eAosSendBlockType_Fixed;


AosTcpProxyTester::AosTcpProxyTester(const OmnIpAddr 	&clientAddr,
							    	 const OmnIpAddr 	&serverAddr,
								     const OmnIpAddr 	&localAddr,
								     const OmnIpAddr 	&bouncerAddr,
								     const int			clientPort,								     
								     const int			numOfTries,
									 const int			connNum,
									 const int			repeat,
									 const int			blockSize)
:
mNumTries(numOfTries),
mClientAddr(clientAddr),   
mServerAddr(serverAddr),   
mLocalAddr(localAddr),    
mBouncerAddr(bouncerAddr),  
mClientPort(clientPort),   
mConnNum(connNum),      
mRepeat(repeat),       
mBlockSize(blockSize)
{                                    
	//                               
	//init                           
	//                               
                                     
	//Repeat                         
	mChangeArray[0].weight = 20;
	mChangeArray[0].min = 1;
	mChangeArray[0].max = 10000;
	mChangeArray[0].last = 1; 
	//mChangeArray[0].choosed = 0;

	//NumConns
   	mChangeArray[1].weight = 20;
	mChangeArray[1].min = 1;
	mChangeArray[1].max = 10000;
	mChangeArray[1].last = 1; 
	//mChangeArray[1].choosed = 0;

	//ContentLen
   	mChangeArray[2].weight = 20;
	mChangeArray[2].min = 1;
	mChangeArray[2].max = 10000;
	mChangeArray[2].last = 1; 
	//mChangeArray[2].choosed = 0;
/*
	//TargetIP
   	mChangeArray[3].weight = 20;
	mChangeArray[3].min = 1;
	mChangeArray[3].max = eAosTcpProxy_TargetIPSelectNum;
	mChangeArray[3].last = 0; 
	//mChangeArray[3].choosed = 0;
	
	//TargetPort
   	mChangeArray[4].weight = 20;
	mChangeArray[4].min = 1;
	mChangeArray[4].max = eAosTcpProxy_TargetPortSelectNum;
	mChangeArray[4].last = 0; 
	//mChangeArray[4].choosed = 0;
*/	
/*
	mVSArray[0].ip = "192.168.10.98";
	mVSArray[0].port = 8001;
	mVSArray[1].ip = "192.168.10.97";
	mVSArray[1].port = 8001;
	mVSArray[2].ip = "192.168.10.96";
	mVSArray[2].port = 8001;

	mVSArray[3].ip = "192.168.10.95";
	mVSArray[3].port = 8001;
	mVSArray[4].ip = "192.168.10.94";
	mVSArray[4].port = 8001;
	mVSArray[5].ip = "192.168.10.93";
	mVSArray[5].port = 8001;
	
	mVSArray[6].ip = "192.168.10.98";
	mVSArray[6].port = 8002;
	mVSArray[7].ip = "192.168.10.97";
	mVSArray[7].port = 8002;
	mVSArray[8].ip = "192.168.10.96";
	mVSArray[8].port = 8002;
	mVSArray[9].ip = "192.168.10.95";
	mVSArray[9].port = 8002;
	mVSArray[10].ip = "192.168.10.94";
	mVSArray[10].port = 8002;
	mVSArray[11].ip = "192.168.10.93";
	mVSArray[11].port = 8002;
	
	mVSArray[12].ip = "192.168.10.98";
	mVSArray[12].port = 8003;
	mVSArray[13].ip = "192.168.10.97";
	mVSArray[13].port = 8003;
	mVSArray[14].ip = "192.168.10.96";
	mVSArray[14].port = 8003;
	mVSArray[15].ip = "192.168.10.95";
	mVSArray[15].port = 8003;
	mVSArray[16].ip = "192.168.10.94";
	mVSArray[16].port = 8003;
	mVSArray[17].ip = "192.168.10.93";
	mVSArray[17].port = 8003;
	
	mVSArray[18].ip = "192.168.10.98";
	mVSArray[18].port = 8004;
	mVSArray[19].ip = "192.168.10.97";
	mVSArray[19].port = 8004;
	mVSArray[20].ip = "192.168.10.96";
	mVSArray[20].port = 8004;
	mVSArray[21].ip = "192.168.10.95";
	mVSArray[21].port = 8004;
	mVSArray[22].ip = "192.168.10.94";
	mVSArray[22].port = 8004;
	mVSArray[23].ip = "192.168.10.93";
	mVSArray[23].port = 8004;
	
	mVSArray[24].ip = "192.168.10.98";
	mVSArray[24].port = 8005;
	mVSArray[25].ip = "192.168.10.97";
	mVSArray[25].port = 8005;
	mVSArray[26].ip = "192.168.10.96";
	mVSArray[26].port = 8005;
	mVSArray[27].ip = "192.168.10.95";
	mVSArray[27].port = 8005;
	mVSArray[28].ip = "192.168.10.94";
	mVSArray[28].port = 8005;
	mVSArray[29].ip = "192.168.10.93";
	mVSArray[29].port = 8005;

	mVSArray[30].ip = "192.168.10.98";
	mVSArray[30].port = 8006;
	mVSArray[31].ip = "192.168.10.97";
	mVSArray[31].port = 8006;
	mVSArray[32].ip = "192.168.10.96";
	mVSArray[32].port = 8006;
	mVSArray[33].ip = "192.168.10.95";
	mVSArray[33].port = 8006;
	mVSArray[34].ip = "192.168.10.94";
	mVSArray[34].port = 8006;
	mVSArray[35].ip = "192.168.10.93";
	mVSArray[35].port = 8006;


	mVSArray[0].ip = "192.168.10.78";
	mVSArray[0].port = 8001;
	mVSArray[1].ip = "192.168.10.77";
	mVSArray[1].port = 8001;
	mVSArray[2].ip = "192.168.10.76";
	mVSArray[2].port = 8001;
	mVSArray[3].ip = "192.168.10.75";
	mVSArray[3].port = 8001;
	mVSArray[4].ip = "192.168.10.74";
	mVSArray[4].port = 8001;
	mVSArray[5].ip = "192.168.10.73";
	mVSArray[5].port = 8001;
	
	mVSArray[6].ip = "192.168.10.78";
	mVSArray[6].port = 8002;
	mVSArray[7].ip = "192.168.10.77";
	mVSArray[7].port = 8002;
	mVSArray[8].ip = "192.168.10.76";
	mVSArray[8].port = 8002;
	mVSArray[9].ip = "192.168.10.75";
	mVSArray[9].port = 8002;
	mVSArray[10].ip = "192.168.10.74";
	mVSArray[10].port = 8002;
	mVSArray[11].ip = "192.168.10.73";
	mVSArray[11].port = 8002;
	
	mVSArray[12].ip = "192.168.10.78";
	mVSArray[12].port = 8003;
	mVSArray[13].ip = "192.168.10.77";
	mVSArray[13].port = 8003;
	mVSArray[14].ip = "192.168.10.76";
	mVSArray[14].port = 8003;
	mVSArray[15].ip = "192.168.10.75";
	mVSArray[15].port = 8003;
	mVSArray[16].ip = "192.168.10.74";
	mVSArray[16].port = 8003;
	mVSArray[17].ip = "192.168.10.73";
	mVSArray[17].port = 8003;
	
	mVSArray[18].ip = "192.168.10.78";
	mVSArray[18].port = 8004;
	mVSArray[19].ip = "192.168.10.77";
	mVSArray[19].port = 8004;
	mVSArray[20].ip = "192.168.10.76";
	mVSArray[20].port = 8004;
	mVSArray[21].ip = "192.168.10.75";
	mVSArray[21].port = 8004;
	mVSArray[22].ip = "192.168.10.74";
	mVSArray[22].port = 8004;
	mVSArray[23].ip = "192.168.10.73";
	mVSArray[23].port = 8004;
	
	mVSArray[24].ip = "192.168.10.78";
	mVSArray[24].port = 8005;
	mVSArray[25].ip = "192.168.10.77";
	mVSArray[25].port = 8005;
	mVSArray[26].ip = "192.168.10.76";
	mVSArray[26].port = 8005;
	mVSArray[27].ip = "192.168.10.75";
	mVSArray[27].port = 8005;
	mVSArray[28].ip = "192.168.10.74";
	mVSArray[28].port = 8005;
	mVSArray[29].ip = "192.168.10.73";
	mVSArray[29].port = 8005;

	mVSArray[30].ip = "192.168.10.78";
	mVSArray[30].port = 8006;
	mVSArray[31].ip = "192.168.10.77";
	mVSArray[31].port = 8006;
	mVSArray[32].ip = "192.168.10.76";
	mVSArray[32].port = 8006;
	mVSArray[33].ip = "192.168.10.75";
	mVSArray[33].port = 8006;
	mVSArray[34].ip = "192.168.10.74";
	mVSArray[34].port = 8006;
	mVSArray[35].ip = "192.168.10.73";
	mVSArray[35].port = 8006;
*/
/*	
	mTargetIP_select[0] = "192.168.10.98";
	mTargetIP_select[1] = "192.168.10.97";
	mTargetIP_select[2] = "192.168.10.96";
	mTargetIP_select[3] = "192.168.10.95";
	mTargetIP_select[4] = "192.168.10.94";
	mTargetIP_select[5] = "192.168.10.93";

	mTargetPort_select[0] = 8001;
	mTargetPort_select[1] = 8002;
	mTargetPort_select[2] = 8003;
	mTargetPort_select[3] = 8004;
	mTargetPort_select[4] = 8005;
	mTargetPort_select[5] = 8006;
*/
}


bool AosTcpProxyTester::start()
{
	// 
	// Test default constructor
	//
	
	//basicTest();
	//attackTest();
	//specialTest();
	tortureTest();
	
	return true;
}


bool AosTcpProxyTester::basicTest()
{ 
	return true;
}


bool AosTcpProxyTester::specialTest()
{ 
	int i;
	OmnBeginTest << "Test AosSlab";
	mTcNameRoot = "TcpProxyTest";
	
	//
	//init special data
	//
	
	//Repeat	
	mChangeArray[0].weight = 30;
	mChangeArray[0].min = 1;
	mChangeArray[0].max = 100;
	mChangeArray[0].last = 1; 
	//mChangeArray[0].choosed = 0;

	//NumConns
   	mChangeArray[1].weight = 40;
	mChangeArray[1].min = 1;
	mChangeArray[1].max = 100;
	mChangeArray[1].last = 100; 
	//mChangeArray[1].choosed = 0;

	//ContentLen
   	mChangeArray[2].weight = 30;
	mChangeArray[2].min = 1;
	mChangeArray[2].max = 8000;
	//mChangeArray[2].last = 271; 
	mChangeArray[2].last = 1024; 
	//mChangeArray[2].choosed = 0;

//	mStrSend = OmnString(mChangeArray[eContentLen].last,'A',true);
//	mNumTries = eAosTcpProxy_NumTries;


//	mStrSend = OmnString(mBlockSize,'A',true);
	char *dataPtr = new char[mBlockSize];
	
	while (mNumTries--)
	{
		cout << "===================LOOP LEFT:" << mNumTries << "===================" << endl;
/*		AosTcpTrafficGen clientGen((mNumTries),
                               OmnIpAddr("192.168.10.88"),//Local_mainIP, 
                               OmnIpAddr("192.168.10.50"),//mClientProxy_IP, 
                               8001,
                               1,
                               mChangeArray[eRepeat].last,
                               mChangeArray[eNumConns].last,
                               dataPtr,//mStrSend,
                               mChangeArray[eContentLen].last,
                               eAosSendBlockType_Fixed,
                               sendBlockSize,
                               mChangeArray[eContentLen].last);
*/
		AosTcpTrafficGen clientGen((mNumTries),
                               mLocalAddr,//Local_mainIP, 
                               mClientAddr,//mClientProxy_IP, 
                               mClientPort,
                               1,
                               mRepeat,
                               mConnNum,
                               dataPtr,//mStrSend,
                               mBlockSize,
                               eAosSendBlockType_Fixed,
                               mBlockSize,
                               mBlockSize);

    	clientGen.start();
    	while(!clientGen.checkFinish())
    	{
        	clientGen.checkConns();
        	OmnSleep(1);
    	}
    	clientGen.stop();
		delete []dataPtr;
		clientGen.printStatus();
    	mSuccessConns = clientGen.getSuccessConns();
    	mFailedConns = clientGen.getFailedConns();
		cout << "=========================================================" << endl;
		cout << "Total Succ:" << mSuccessConns << "  , Total Fail:" << mFailedConns << endl;
		cout << "=========================================================" << endl;

		checkResults();
	}	
	return true;
}


bool AosTcpProxyTester::tortureTest()
{
	OmnBeginTest << "Test AosSlab";
	mTcNameRoot = "TcpProxyTest";
    if (mNumTries <= 0)
	{
		mNumTries =  eAosTcpProxy_NumTries;
		cout<<"info:not set trynum,the testpkg will use the default setting"<<endl;
	}
	//mNumTries = eAosTcpProxy_NumTries;
	//int tmpnum = eAosTcpProxy_NumTries;

	while (mNumTries--)
	{
		cout << "=============================LOOP LEFT:" << mNumTries << "===========================" << endl;
		mChangeArray[0].choosed = 0;
		mChangeArray[1].choosed = 0;
		mChangeArray[2].choosed = 0;
		generateData();
		doOperations();
		checkResults();
	}
 
	return true;
}


bool AosTcpProxyTester::generateData()
{
	int i, j, weightsum, opt_size, opt_change, opt_changenum;

	opt_changenum = i = OmnRandom::nextInt(1, eAosTcpProxy_ChangesNum);
	while( i )
	{
		weightsum = 0;
		for(j = 0; j < eAosTcpProxy_ChangesNum; j++)
		{
			if(!mChangeArray[j].choosed)
			{
				weightsum += mChangeArray[j].weight;
			}
		}
		opt_change = OmnRandom::nextInt(1, weightsum);
		for(j = 0; j < eAosTcpProxy_ChangesNum; j++)
		{
			if((!mChangeArray[j].choosed)&&(opt_change <= mChangeArray[j].weight))
			{
				mChangeArray[j].choosed = 1;
				break;
			}
			else
			{
				opt_change -= mChangeArray[j].weight;
			}
		}
		i--;
	}
	
	for( i = 0; i< eAosTcpProxy_ChangesNum; i++ )
	{
		if(mChangeArray[i].choosed)
		{
			if(i==eContentLen)
			{
				opt_size = OmnRandom::nextInt(1, 100);
				if(opt_size <= eWeightLess256)
				{
					mChangeArray[i].last = OmnRandom::nextInt(1, 256);	
				}
				else if(opt_size <= eWeightLess1024)
				{
					mChangeArray[i].last = OmnRandom::nextInt(257, 1024);	
				}
				else if(opt_size <= eWeightLess10K)
				{
					mChangeArray[i].last = OmnRandom::nextInt(1025, 10240);	
				}
				else if(opt_size <= eWeightLess1M)
				{
					mChangeArray[i].last = OmnRandom::nextInt(10241, 1048576);	
				}
				else
				{
					mChangeArray[i].last = OmnRandom::nextInt(1048577, 104857600);	
				}
			}		
			else 
			{
				mChangeArray[i].last = OmnRandom::nextInt(mChangeArray[i].min, mChangeArray[i].max);	
			}
		}
	}
	
	for( i = 0; i< mChangeArray[eContentLen].last; i++ )
	{
		mStrSend += "A";	
	}
	
	//cout << "The send strlen=" << mChangeArray[eContentLen].last << endl;
	//cout << "The content=" << mStrSend  << endl;
	
    /*
	cout << "===================================================================" << endl;
	cout << "The test data: Num=" << mChangeArray[eNumConns].last << ", Repeat=" << mChangeArray[eRepeat].last << ", ContentLen=" << mChangeArray[eContentLen].last <<  endl;
	*/
	//modified by <a href=mailto:xw_cn@163.com>xiawu</a>
	 cout << "===================================================================" << endl;
	 cout << "The test data: Num=" << mConnNum<< ", Repeat=" <<mRepeat<< ", ContentLen=" << mChangeArray[eContentLen].last <<  endl;
	//end of modified
	return true;
}


bool AosTcpProxyTester::doOperations()
{
	//int TargetVS = OmnRandom::nextInt(1, eAosTcpProxy_TargetVSNum);
	//cout << " VS=> " << mVSArray[TargetVS].ip << ":" << mVSArray[TargetVS].port << endl;
	//cout << "===================================================================" << endl;
	
	//modified by <a href=mailto:xw_cn@163.com>xiawu</a>
	if (mBlockSize <= 0)
	{
		cout<<"info:parameter size is not set,the default setting size="<<sendBlockSize<<" will be used."<<endl;
		mBlockSize =  sendBlockSize;
	}
	//end of modified
	AosTcpTrafficGen clientGen((mNumTries),
                               mLocalAddr,//OmnIpAddr("192.168.2.239"),
                               mClientAddr,//OmnIpAddr("192.168.2.219"),//mClientProxy_IP, 
                               mClientPort,//8001,
							   //OmnIpAddr(mVSArray[TargetVS].ip),
							   //mVSArray[TargetVS].port,
                               1000,
                               mRepeat,//mChangeArray[eRepeat].last,
                               mConnNum,//mChangeArray[eNumConns].last,
                               mStrSend,//"00000000user login:test:test$#",
                               mChangeArray[eContentLen].last,//30,
                               eAosSendBlockType_Fixed,
                               mBlockSize,//sendBlockSize,
                               //"00000000user login:test:test$#");
							   mChangeArray[eContentLen].last);// 30);
							  // mContentLen);

//    AosTrafficGenCallback *genCallback = OmnNew AosTrafficGenCallback(this);
//   AosTcpTrafficGenListenerPtr genListener = genCallback;
//    clientGen.registerCallback(genListener);
	//clientGen.addLocalAddr(OmnIpAddr("192.168.10.87"));
	//clientGen.addLocalAddr(OmnIpAddr("192.168.10.89"));
	//cout<<"repeat num is "<<mRepeat<<endl;
	//cout<<"concurrent num is "<<mConnNum<<endl;
    
	clientGen.start();
    while(!clientGen.checkFinish())
    {
        clientGen.checkConns();
        OmnSleep(1);
    }
	clientGen.stop();	
	clientGen.printStatus();
    mSuccessConns = clientGen.getSuccessConns();
    mFailedConns = clientGen.getFailedConns();
	cout << "=========================================================" << endl;
	cout << "Total Succ:" << mSuccessConns << "  , Total Fail:" << mFailedConns << endl;
	cout << "=========================================================" << endl;

	return true;
}


bool AosTcpProxyTester::checkResults()
{

	// Test the constructor
	//AosTimerPack_t * pack = AosTimerPack_create(10, AosTimerPackTestCallback);

	OmnTC(OmnExpected<int>(mChangeArray[eNumConns].last*mChangeArray[eRepeat].last), OmnActual<int>(mSuccessConns)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(mFailedConns)) << endtc;

//	while(i>0)
//	{
//		OmnSleep(1);	//	sleep(mfreq_msec);

//		i--;
//	}
	return true;
}

bool AosTcpProxyTester::attackTest()
{ 
	//commence_syn (mTarget, mSynPort);
	commence_syn("192.168.100.188", "192.168.10.78", 8001);
	commence_fin("192.168.100.188", "192.168.10.78", 8001);
	commence_rst("192.168.100.188", "192.168.10.78", 8001);
	commence_ack("192.168.100.188", "192.168.10.78", 8001);
	
	commence_icmp("192.168.10.88", "192.168.10.78");
	commence_udp("192.168.10.88", "192.168.10.78", 8001);

	commence_syn("192.168.100.88", "192.168.100.98", 8001);
	commence_fin("192.168.100.88", "192.168.100.98", 8001);
	commence_rst("192.168.100.88", "192.168.100.98", 8001);
	commence_ack("192.168.100.88", "192.168.100.98", 8001);
	return true;
}
	

