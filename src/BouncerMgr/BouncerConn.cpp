////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BouncerConn.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "BouncerMgr/BouncerConn.h"

#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "Random/RandomUtil.h"

AosBouncerConn::AosBouncerConn(const OmnIpAddr 		&senderIp,
							   const int		  	&senderPort,
							   const OmnIpAddr 		&recverIp,
							   const int		  	&recverPort,
							   const OmnString 		&protocol)
:
mSenderIp(senderIp),
mSenderPort(senderPort),
mRecverIp(recverIp),
mRecverPort(recverPort),
mProtocol(protocol),
mLastPos(0),
mLastSecTick(0),
mLastSlotTick(0),
mStartTimeSec(OmnGetSecond()),
mStartTimeUsec(OmnGetUsec()),
mTotalVolumn(0),
mConnTag(RAND_INT(1,10000)),
mLock(OmnNew OmnMutex())
{
	memset(mCollector,0,eCollectorLen * sizeof(int));
}

AosBouncerConn::~AosBouncerConn()
{
}
	
bool	
AosBouncerConn::connExist(const OmnIpAddr 		&senderIp,
		  		  		  const int		  		&senderPort)
{
	return (senderIp == mSenderIp && senderPort == mSenderPort);
}		  		  		  

bool	
AosBouncerConn::getBandwidth(int	&curBandwidth,
			     	 		 int	&avgBandwidth)
{
	addVolume(0);

	mLock->lock();
	
	//1. get cur bandwidth

/*
	if(0 == mLastPos)
	{
		curBandwidth = mCollector[eCollectorLen - 1];
	}
	else
	{
		curBandwidth = mCollector[mLastPos - 1];
	}
*/

	curBandwidth = 0;
	for(int i = 0;i < eCollectorLen;i++)
	{
		curBandwidth += mCollector[i];
	}
	//2. get avgBandwidth
	int interval = OmnGetSecond() - mStartTimeSec;
	interval *= 1000;
	int intervalUsec = OmnGetUsec() - mStartTimeUsec;
	interval += (intervalUsec/1000);
		
	if(interval <= 0)
	{
		avgBandwidth = mTotalVolumn;
	}
	else
	{
		float tmpvalue = (float)mTotalVolumn;
		tmpvalue = tmpvalue / interval;
		tmpvalue = tmpvalue * 1000;		
		avgBandwidth = (int)tmpvalue;
	}
	mLock->unlock();
	return true;
}			     	 

bool	
AosBouncerConn::resetStat()
{
	mLock->lock();


	mStartTimeSec = OmnGetSecond();//OmnTime::getSecTick();
	mStartTimeUsec = OmnGetUsec();
	mLastSecTick = mStartTimeSec;
	mLastSlotTick = 0;
	memset(mCollector,0,eCollectorLen * sizeof(int));
	mTotalVolumn = 0;
	mLastPos = 0;

	mLock->unlock();
	return false;
}

bool	
AosBouncerConn::getPacket(const OmnConnBuffPtr &buff,const OmnString &proto)
{
	if(proto == "udp")
	{
		return addVolume(buff->getDataLength() + 42);
	}
	// else "tcp"
	return addVolume(buff->getDataLength() + 54);
}

void
AosBouncerConn::clearExpiredSlots()
{
	int tick = OmnTime::getSecTick();
OmnTrace << "tick is : " << tick << endl;
	int slot = OmnGetUsec() * eCollectorLen /1000000;
	// int slotsToJump = (tick - mLastSecTick)*eCollectorLen + slot - mLastSlotTick;
	int slotsToJump = (tick - mLastSecTick)*eCollectorLen + slot - mLastPos;
	if(slotsToJump > eCollectorLen)
	{
		slotsToJump = eCollectorLen;
	}
	for(int i = 0;i < slotsToJump;i++)
	{
		mLastPos ++;
		if(mLastPos >= eCollectorLen)
		{
			mLastPos = 0;
		}
		mCollector[mLastPos] = 0;
	}
}

bool
AosBouncerConn::addVolume(const int size)
{
	mLock->lock();
	clearExpiredSlots();
	mCollector[mLastPos] += size;
	mLastSecTick = OmnTime::getSecTick();
/*	//1. clear expired slots.
	int interval = tick - mLastTimeTick;
	if(interval < 0)
	{
		// should not happen
		mLock->unlock();
		return false;
	}
	
	if(interval > eCollectorLen)
	{
		interval = eCollectorLen;
	}
	
	
	for(int i = 0;i < interval;i++)
	{
		mCollector[mLastPos] = 0;
		mLastPos ++;
		if(mLastPos >= eCollectorLen)
		{
			mLastPos = 0;
		}
	} 
	
	mCollector[mLastPos] += size;


	
	mLastTimeTick = tick;
*/

	mTotalVolumn += size;

	mLock->unlock();
	return true;
}	

