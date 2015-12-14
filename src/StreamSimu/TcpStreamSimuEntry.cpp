////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpStreamSimuEntry.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "StreamSimu/TcpStreamSimuEntry.h"

#include "Alarm/Alarm.h"
#include "StreamSimu/StreamSimuConn.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Random/RandomUtil.h"
#include "Util1/Wait.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"

AosTcpStreamSimuEntry::AosTcpStreamSimuEntry(const OmnIpAddr& senderIp, 
										     const int senderPort)
:
AosStreamSimuEntry::AosStreamSimuEntry(senderIp,senderPort,"tcp", 0),
mConnOn(false),
mRecverPort(-1),
mType(AosStreamType::eNormal),
mMaxBw(0),
mMinBw(0),
mPacketNum(0),
mCurBwPos(0),
mConnected(false)
{
	memset(mBwList,0,eBwListSize*sizeof(int));

    OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "TimeThread", 0, true, true, __FILE__, __LINE__);
}
	
AosTcpStreamSimuEntry::~AosTcpStreamSimuEntry()
{
}	

bool	
AosTcpStreamSimuEntry::start()
{
OmnTrace << "1" << endl;
	mTcpClient = OmnNew OmnTcpClient(mRecverIp,
									 mRecverPort,
									 1,
									 mSenderIp,
									 mSenderPort,
									 1,
									 "TcpStreamSimu");


	mStatus = eSuccess;
OmnTrace << "1" << endl;
	mThread->start();	
OmnTrace << "1" << endl;
	
	return true;
}

bool	
AosTcpStreamSimuEntry::stop()
{
	// need to stop reading
	mThread->stop();
	while(mThread->isStopped())
	{
    	OmnWait::getSelf()->wait(0,10000);		
	}
	mThread = 0;	
	mTcpClient->closeConn();
	return true;
}

bool    
AosTcpStreamSimuEntry::sendPacket(const OmnIpAddr &recverIp, 
								  const int recverPort,
								  const int size)
{
	if(size < 0 || size > eMaxPacketSize)
	{
		return false;
	}
	mLock->lock();
OmnTrace << "To send: " << mSenderIp.toString() << ":" << mSenderPort << " -> " << recverIp.toString() << ":" << recverPort << endl;
	mTcpClient->setBlockingType(false);
	if(!mTcpClient->writeTo(mContent,size))
	{
		mTcpClient->closeConn();
		OmnString err;
		if(!mTcpClient->connect(err))
		{
			OmnTrace << "can not connect:" << err << endl;
		}
	}
	mLock->unlock();
	return false;
}

// we set the information into entry instead of create a real conn
bool	
AosTcpStreamSimuEntry::addConn(const OmnIpAddr 						&recverIp,
							   const int		  					recverPort,
							   const int		  					bandwidth,
						       const int		  					packetSize,
                               const AosStreamType::E 				streamType,
                     	       const int                          	maxBw,
                               const int                          	minBw)
{
OmnTrace << "addConn: " << mSenderIp.toString() << ":" << mSenderPort << " -> " << recverIp.toString() << ":" << recverPort << endl;
OmnTrace << "bandwith: " << bandwidth << endl;
	mRecverIp 	= recverIp;
	mRecverPort = recverPort;

	mBandwidth 	= bandwidth;
	mPacketSize = packetSize;

	mType		= streamType;
	mMaxBw		= maxBw;
	mMinBw		= minBw;

	mConnOn = true;

	adjustParms();

	return true;
	
}
			
// we set mConnOn flag instead of remove a real conn (we haven't) 
bool	
AosTcpStreamSimuEntry::removeConn(const OmnIpAddr 		&recverIp,
							   	  const int		  		recverPort)
{
	if(recverIp == mRecverIp && 
	   recverPort == mRecverPort)
	{
		mConnOn = false;
		return true;
	}
	
	// else 
	return false;
}

// we set mConnOn flag instead of remove a real conn (we haven't) 
bool
AosTcpStreamSimuEntry::getBandwidth(const OmnIpAddr 		&recverIp,
							   	 	const int		  		recverPort,
								 	int			  			&bw,
								 	int			  			&packetNum)
{
	if(recverIp		== mRecverIp && 
	   recverPort 	== mRecverPort)
	{
		bw 			= mBandwidth;
		packetNum 	= mPacketNum;
		return true;
	}
	return false;
}


int		
AosTcpStreamSimuEntry::connNum()
{
	return mConnOn;
}


bool	
AosTcpStreamSimuEntry::modifyStream(const OmnIpAddr 					&recverIp,
								    const int		  					recverPort,
								    const int		  					bandwidth,
								    const int		  					packetSize,
		                            const AosStreamType::E 				streamType,
		                     	    const int                          	maxBw,
		                            const int                          	minBw)
{
	if(recverIp == mRecverIp && 
	   recverPort == mRecverPort)
	{
		mBandwidth = bandwidth;
		mPacketSize = packetSize;
	}

	adjustParms();
	return false;	
}


bool	
AosTcpStreamSimuEntry::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter AosStreamSimuConn::threadFunc." << endl;

    while (state == OmnThrdStatus::eActive)
    {
OmnTrace << "1" << endl;
		if(!mTcpClient->isConnGood())
		{
OmnTrace << "To send but Not connected" << endl;
			OmnString strErr;
			if(!mTcpClient->connect(strErr))
			{
				mStatus = eFailedCreateConn;
            	OmnWait::getSelf()->wait(1,0);
				continue;
			}
			mConnected = true;
		}

    	// send a packet
OmnTrace << "1" << endl;
		mLock->lock();
		int sendSize = 	mBwList[mCurBwPos];
		mCurBwPos++;
		if(mCurBwPos >= eBwListSize)
		{
			mCurBwPos = 0;
		}

/*        int tail = mBandwidth % 100;

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
        mLock->unlock();

OmnTrace << "1" << endl;
        while(sendSize > mPacketSize)
        {
OmnTrace << "1" << endl;
            sendPacket(mRecverIp,mRecverPort,mPacketSize);
            sendSize -= mPacketSize;
        }

OmnTrace << "1: " << sendSize << ":" << mPacketSize << ":" << mBandwidth << endl;
		if (sendSize > 0)
		{
OmnTrace << "1" << endl;
        	sendPacket(mRecverIp,mRecverPort,sendSize);
OmnTrace << "1" << endl;
		}

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

OmnTrace << "1" << endl;
		
/*    	sendPacket(mRecverIp,mRecverPort,mPacketSize);
    	// wait a while
    	mLock->lock();
    	int psize = mPacketSize;
    	int bw = mBandwidth;
   		int interval = mPacketSize*1000000/mBandwidth;
    	mLock->unlock();
    	if(0 == bw || 0 == psize)
    	{
    		// error condition
    		OmnWait::getSelf()->wait(1,0);
    	}
    	else
    	{
	    	mLock->unlock();
    		OmnWait::getSelf()->wait(0,interval);
    	}
*/
    }

    OmnTrace << "Leave AosStreamSimuConn::threadFunc" << endl;
    return true;
}

bool	
AosTcpStreamSimuEntry::signal(const int threadLogicId)
{
	return true;
}

bool    
AosTcpStreamSimuEntry::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

void
AosTcpStreamSimuEntry::adjustParms()
{
	int i = 0;
	int total = 0;
	int tail = 0;
	int blockSize = 0;
OmnTrace << "adjustParms: " << mSenderIp.toString() << ":" << mSenderPort << endl;
	switch(mType)
	{
		case AosStreamType::eNormal:
			//1. set mBandwidth;
			//2. set mBwList
			tail = mBandwidth % eBwListSize;
			blockSize = mBandwidth / eBwListSize;
OmnTrace << "Blocksize: " << blockSize << ":" << tail << endl;
			mPacketNum = 0;
			for(i = 0;i < eBwListSize; i++)
			{
				mBwList[i] = (tail)?blockSize+1:blockSize;
				mPacketNum += calcPks(mBwList[i]);
				tail--;
			}
			break;

		case AosStreamType::eWave:
OmnTrace << "1" << endl;
			//1. set mBandwidth;
/*
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
*/
			OmnAlarm << "Not implemented yet" << enderr;
			break;
			
		case AosStreamType::eBlock:
			//1. set mBandwidth;
/*
OmnTrace << "**********" << endl;
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
*/
			OmnAlarm << "Not implemented yet" << enderr;
			break;

		case AosStreamType::eRandom:
			total = 0;
			mPacketNum = 0;
			for(i = 0;i < eBwListSize;i++)
			{
				mBwList[i] = RAND_INT(mMinBw/eBwListSize, mMaxBw/eBwListSize);
				if (mBwList[i] == 0) mBwList[i] = RAND_INT(1, eMinBytePerSlot);
				total += mBwList[i];
				mPacketNum += calcPks(mBwList[i]);
			}
			mBandwidth = total;
			break;

		default:
			OmnAlarm << "Incorrect type: " << mType << enderr;
			break;
	}
OmnTrace << ":" << endl;
}


// 
// Calculate how many packets to send for 'sendSize' number of bytes. 
// 
int
AosTcpStreamSimuEntry::calcPks(const int sendSize)
{
	if(sendSize <= 0 || mPacketSize <= 0)
	{
		return 0;
	}

	if (sendSize % mPacketSize) return sendSize/mPacketSize + 1;
	return sendSize/mPacketSize;
}
