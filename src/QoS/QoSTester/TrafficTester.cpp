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
// This torturer tests the QoS traffic control functions. This can be considered
// as the overall torturer (except traffic shaping). The configuration of this
// torturer is:
//         ---------           ----------         ----------       -----------
//         |  TG   |-----------|  TC    |---------| RHC    |-------| Bouncer |
//         ---------           ----------         ----------       ----------- 
//
// Below are the torturing controlling variables:
//   1. TC: constant/variable value
//   2. number of connections
//   3. priority of each conn
//   4. connection bandwidth
//   5. connection duration
//   6. bouncing method
//   7. rhc config
//   8. tcp/udp
//   
// The torturing idea is: 
//   1. Use TC to limit the WAN bandwidth 
//   2. Configure the RHC with some high priority rules
//   3. Initiate a number of connections, some are high priority and some low 
//      priority. 
//   4. Monitor the bandwidth usage of each connection. Check whether they are
//      as expected. 
//
// Modification History:
// 01/10/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "QoS/QoSTester/TrafficTester.h"

#include "Alarm/Alarm.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpBouncer.h"


AosQosTrafficTester::AosQosTrafficTester()
{
	mName = "AosQosTrafficTester";
}


bool AosQosTrafficTester::start()
{
	unsigned int ret;
	// 
	// Test default constructor
	//

	mLocalIP = OmnIpAddr("127.0.0.1");
	mLocalPort = 5000;
	mNumLocalPorts = 5;
	mMaxConns = 1000;
	mBounceType = AosTcpBouncer::eSimpleBounce;
	
//	basicTest();
	return true;
}

/* Commented out by Chen Ding, 02/26/2007
bool 
AosQosTrafficTester::basicTest()
{
	startBouncer();
}	


bool 
AosQosTrafficTester::startBouncer()
{
	mBouncer = OmnNew AosTcpBouncer("QoSTraffic", mLocalIP, mLocalPort, 
					mNumLocalPorts, mMaxConns, 
					(AosTcpBouncer::BounceType)mBounceType, 0);

	OmnString errmsg;
	if (!mBouncer->connect(errmsg))
	{
		OmnAlarm << "Failed to connect: " << errmsg << enderr;
		return false;
	}

	mBouncer->startReading();
	return true;
}	


bool
AosQosTrafficTester::testBandwidth()
{
	// 
	//   1. TC: constant/variable value
	//   2. number of connections
	//   3. priority of each conn
	//   4. connection bandwidth
	//   5. connection duration
	//   6. bouncing method
	//   7. rhc config
	//   8. tcp/udp
	//
	
	int tries = 0;
	while (tries < 10)
	{
		// 
		// 1. Determine the TC value. 
		//
		configureTc();
		startConns();
		configureRhc();

		OmnSleep(1);
		tries++;
	}

	return true;
}


bool
AosQosTrafficTester::configureTc()
{
	if (OmnTime::getCrtSec() - mLastTcSec < mTcDuration)
	{
		// 
		// No need to reconfigure the TC yet.
		//
		return true;
	}

	// 
	// Time to re-configure the TC
	//
	mTcDuration = mTcDurationSelector.nextInt();
	mTcBand = mTcBandSelector.nextInt();
	// actually configure the TC
	mLastTcSec = OmnTime::getCrtSec();
	return true;
}


bool
AosQosTrafficTester::startConns()
{
	// 
	// All the current connections are stored in mConns. 
	// We need to randomly determine how many connections
	// it should be this time. 
	//
	// First, we check whether we need to add/delete connections
	// now. This is randomly determined.
	//
	if (OmnTime::getCrtSec - mLastConnChanged < mConnChangeDuration)
	{
		// 
		// Not the time to change the connections yet.
		//
		return true;
	}

	// 
	// Randomly determine how many connections it should be
	//
	int numConns = OmnRandom::nextInt(1, mMaxConns);

	// 
	// Randomly determine the next time to change the connections
	//
	mConnChangeDuration = OmnRandom::nextInt(eConnChangeMin, 
			eConnChangeMax);
	mLastConnChanged = OmnTime::getCrtSec();

	if (numConns == mNumConns)
	{
		// 
		// No changes. Return.
		//
		return true;
	}

	if (numConns < mNumConns)
	{
		// 
		// Don't need to do anything. These connections will 
		// finish on their own.
		//
		return true;
	}

	// 
	// Need to add more connections
	//
	for (int i=0; i<numConns-mNumConns; i++)
	{
		mConns[mNumConns+i].priority = mPrioritySelector.nextInt();
		mConns[mNumConns+i].bandwidth = mBandwidthSelector.nextInt();
		mConns[mNumConns+i].duration = mDurationSelector.nextInt();
		startConn(mConns[mNumConns+i]);
	}

	mNumConns = numConns;
	return true;
}


bool
AosTrafficGenTester::startConn()
{
}
*/

