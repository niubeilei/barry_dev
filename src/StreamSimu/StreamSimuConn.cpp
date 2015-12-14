////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamSimuConn.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "StreamSimu/StreamSimuConn.h"

#include "StreamSimu/StreamSimuEntry.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Random/RandomUtil.h"
#include "Util1/Time.h"
#include "Util1/Wait.h"
#include "UtilComm/ConnBuff.h"

AosStreamSimuConn::AosStreamSimuConn(const OmnIpAddr 						&senderIp,
								     const int		  						senderPort,
								     const OmnIpAddr 						&recverIp,
								     const int		  						recverPort,
								     const OmnString 						&protocol,
								     const int		  						bandwidth,
						  		     const int		  						packetSize,
				                     const AosStreamType::E 				streamType,
				             	     const int                          	maxBw,
				                     const int                          	minBw,
						  		     const AosStreamSimuEntryPtr			entry)
:
mSenderIp(senderIp),
mSenderPort(senderPort),
mRecverIp(recverIp),
mRecverPort(recverPort),
mProtocol(protocol),
mBandwidth(bandwidth),
mPacketSize(packetSize),
mLock(OmnNew OmnMutex()),
mEntry(entry),
mType(streamType),
mMaxBw(maxBw),
mMinBw(minBw),
mPacketNum(0),
mCurBwPos(0)
{
OmnTrace << "To add a connection: " << recverIp.toString() << ":" << recverPort << endl;
	adjustParms();
}

AosStreamSimuConn::~AosStreamSimuConn()
{
}
	
bool	
AosStreamSimuConn::connExist(const OmnIpAddr 		&senderIp,
		  		  		  const int		  		&senderPort)
{
	return (senderIp == mSenderIp && senderPort == mSenderPort);
}		  		  		  

bool	
AosStreamSimuConn::modifyStream(const int							bandwidth,
				     	 		const int							packetSize,
		                 		const AosStreamType::E 				streamType,
		             	 		const int                          	maxBw,
		                 		const int                          	minBw)
{
    mLock->lock();
	mBandwidth = bandwidth;
	mPacketSize = packetSize;

	mType		= streamType;
	mMaxBw		= maxBw;
	mMinBw		= minBw;

	adjustParms();
    mLock->unlock();
	return true;
}			     	 

bool	
AosStreamSimuConn::start()
{
    OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "TimeThread", 0, true, true, __FILE__, __LINE__);
	mThread->start();	

	return true;
}

bool	
AosStreamSimuConn::stop()
{
	mThread->stop();
	while(mThread->isStopped())
	{
    	OmnWait::getSelf()->wait(0,10000);		
	}
	mThread = 0;	
	return true;
}

bool	
AosStreamSimuConn::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter AosStreamSimuConn::threadFunc." << endl;

    while (state == OmnThrdStatus::eActive)
    {

    	mLock->lock();
/*		int tail = mBandwidth % 100;
	
		// calc out the packet 10 ms need to send
	
		int sendSize = mBandwidth / 100;
		if(count >= 100)
		{
			count = 0;
			
		}
		else
		{
			count ++;
		}		
		sendSize += tail;
*/
		int sendSize = 	mBwList[mCurBwPos];
		mCurBwPos ++;
		if(mCurBwPos >= 100)
		{
			mCurBwPos = 0;
		}
		mLock->unlock();
		
		while(sendSize > mPacketSize)
		{
	    	mEntry->sendPacket(mRecverIp,mRecverPort,mPacketSize);
			sendSize -= mPacketSize;
		}
    	mEntry->sendPacket(mRecverIp,mRecverPort,sendSize);
	

    	// wait a while
   		//int interval = mPacketSize*1000000/mBandwidth;
    	if(0 == mBandwidth || 0 == mPacketSize)
    	{
    		// error condition
    		OmnWait::getSelf()->wait(1,0);
    	}
    	else
    	{
    		OmnWait::getSelf()->wait(0,10000);
    	}
    	// wait a while
    }

    OmnTrace << "Leave AosStreamSimuConn::threadFunc" << endl;
    return true;
}

bool	
AosStreamSimuConn::signal(const int threadLogicId)
{
	return true;
}

bool    
AosStreamSimuConn::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosStreamSimuConn::getBandwidth(int			  &bw,
								int			  &packetNum)
{
	bw = mBandwidth;
	packetNum = mPacketNum;
	return true;
}

void
AosStreamSimuConn::adjustParms()
{
	int i = 0;
	int total = 0;
	int tail = 0;
	int blockSize = 0;
	switch(mType)
	{
		case AosStreamType::eNormal:
			//1. set mBandwidth;
			//2. set mBwList
			tail = mBandwidth % 100;
			blockSize = mBandwidth / 100;

			for(i = 0;i < 1000;i++)
			{
				mBwList[i] = blockSize;
			}
			for(i = 1;i < 11;i++)
			{
				mBwList[i*100-1] += tail;
			}			
			//3. set mPacketNum
			mPacketNum = (calcPks(mBwList[0])) * 99;
			mPacketNum += calcPks(mBwList[99]);
			break;
		case AosStreamType::eWave:
			//1. set mBandwidth;
			for(i = 0;i < 500;i++)
			{
				mBwList[i] = (mMinBw + (mMaxBw - mMinBw)*i/499)/100;
				mBwList[999-i] = mBwList[i];
			}
			//2. set mBwList
			total = 0;
			mPacketNum = 0;
			for(i = 0;i < 1000;i++)
			{
				total += mBwList[i];
				mPacketNum += calcPks(mBwList[i]);
			}
			
			mBandwidth = total/10;
			

			break;
		case AosStreamType::eBlock:
			//1. set mBandwidth;
			for(i = 0;i < 500;i++)
			{
				mBwList[i] = mMinBw/100;
				mBwList[500+i] = mMaxBw/100;
			}
			//2. set mBwList
			total = 0;
			mPacketNum = 0;
			for(i = 0;i < 1000;i++)
			{
				total += mBwList[i];
				mPacketNum += calcPks(mBwList[i]);
			}
			mBandwidth = total/10;

			break;

		case AosStreamType::eRandom:
			total = 0;
			mPacketNum = 0;
			for(i = 0;i < 1000;i++)
			{
				mBwList[i] = RAND_INT(mMaxBw/100,mMinBw/100);
				total += mBwList[i];
				mPacketNum += calcPks(mBwList[i]);
			}
			mBandwidth = total/10;
			break;

		default:
			break;
	}
}

int
AosStreamSimuConn::calcPks(const int sendSize)
{
	if(mPacketSize <= 0)
	{
		return 0;
	}
	return (sendSize+1)/mPacketSize;
}
