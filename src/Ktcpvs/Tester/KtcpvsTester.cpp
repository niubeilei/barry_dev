////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KtcpvsTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "KtcpvsTester.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpBouncer.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpTrafficGenClient.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpClient.h"
#include "Porting/Sleep.h"

#include "Ktcpvs/Tester/BouncerCallback.h"
#include "Ktcpvs/Tester/TrafficGenCallback.h"


extern bool AosKtcpvsTesterIsGeneratorFlag;
extern bool AosKtcpvsTesterIsBouncerFlag;

int AosKtcpvsTester::mRepeat = 0;
int	AosKtcpvsTester::mNumConns = 0;
int	AosKtcpvsTester::mContentLen = 0;

bool AosKtcpvsTester::start()
{
	// 
	// Test default constructor
	//
	if (AosKtcpvsTesterIsGeneratorFlag)
	{
		OmnTrace << "To call generator" << endl;
		processGenerator();	
	}

	if (AosKtcpvsTesterIsBouncerFlag)
	{
		processBouncer();
	}

	return true;
}

bool AosKtcpvsTester::processGenerator()
{
	OmnBeginTest <<"Test Generate Tracffic";
	mTcNameRoot = "Test_Generate_Traffic";

	OmnTrace << "This is Ktcpvs test-Generate: " << mNumConns << endl;


/*
	//
	// 	Traffic Generator instance 
	//
	int maxConns = 1000;
	AosTcpBouncer theBouncer("TcpBouncer", 
        OmnIpAddr("192.168.0.82"), 5000, 5, 
		maxConns, AosTcpBouncer::eSimpleBounce); 
    OmnString errmsg;
    if (!theBouncer.connect(errmsg))
    {
        cout << "********* Failed to connect: " << errmsg << endl;
        return -1;
    }

	AosTcpBouncerListenerPtr bouncerCallback = OmnNew AosBouncerCallback();
	theBouncer.registerCallback(bouncerCallback);
	theBouncer.startReading();
*/

	//
	// Traffic Generator instance 
	//
	AosTcpTrafficGen gen(OmnIpAddr("192.168.0.82"), 5000, 5,
		mRepeat, mNumConns, 'a', mContentLen);

	AosTrafficGenCallback *genCallback = OmnNew AosTrafficGenCallback(this);
	AosTcpTrafficGenListenerPtr genListener = genCallback;
	gen.registerCallback(genListener);

	gen.start();

	while(!genCallback->isFinished())
	{
		OmnSleep(1);
	}

	return true;	
}

bool AosKtcpvsTester::processBouncer()
{
	OmnBeginTest <<"Test Bouncer Tracffic";
    mTcNameRoot = "Test_Bouncer_Traffic";

    OmnTrace << "This is Ktcpvs test-Bouncer" << endl;	

	//
	//	Traffic Bouncer instance
	//	

    int maxConns = 1000;
    AosTcpBouncer theBouncer("TcpBouncer",
    OmnIpAddr("192.168.0.81"), 5000, 5,
                maxConns, AosTcpBouncer::eSimpleBounce);
	OmnString errmsg;
    if (!theBouncer.connect(errmsg))
    {
        cout << "********* Failed to connect: " << errmsg << endl;
        return -1;
    }

    AosTcpBouncerListenerPtr bouncerCallback = OmnNew AosBouncerCallback();
    theBouncer.registerCallback(bouncerCallback);
    theBouncer.startReading();
	
	return true;

}

void    
AosKtcpvsTester::trafficGenFinished(
			OmnVList<AosTcpTrafficGenClientPtr> &clients)
{
	clients.reset();
	while (clients.hasMore())
	{
		AosTcpTrafficGenClientPtr client = clients.next();
		OmnTC(OmnExpected<int>(mRepeat), 
			  OmnActual<int>(client->repeated())) << endtc;

		OmnTC(OmnExpected<int>(mContentLen), 
			  OmnActual<int>(client->received())) << endtc;
	}

	OmnTC(OmnExpected<int>(mNumConns), OmnActual<int>(clients.entries())) << endtc;
}
	
