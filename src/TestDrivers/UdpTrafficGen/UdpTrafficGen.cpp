////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpTrafficGen.cpp
// Description:
// 12/07/2007: Created by Allen Xu
//   
//
// Modification History:
//
////////////////////////////////////////////////////////////////////////////
#include "UdpTrafficGen.h"

#include "Util/OmnNew.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"
#include "Alarm/Alarm.h"
#include "Thread/Thread.h"
#include "Porting/Sleep.h"
#include "sys/types.h"
#define MOD_VALUE 10

AosUdpTrafficGen::AosUdpTrafficGen(
		const OmnIpAddr &localAddr, 
		const int localPort,
		const OmnIpAddr &remoteAddr, 
		const int remotePort,
		const OmnString &filename, 
		const int packetSize,
		const int sendRate,
		const unsigned int maxPackets,
		const int breakTime)
:
mFileName(filename),
mLocalAddr(localAddr),
mLocalPort(localPort),
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mPacketSize(packetSize),
mSendRate(sendRate),
mMaxPackets(maxPackets),
mBreakTime(breakTime)
{
	mPacketSent=0;
	mPacketReceived=0;
	mBytesSent=0;
	mBytesReceived=0;

	pthread_mutex_init(&mMutex, NULL);
	pthread_cond_init(&mCond,NULL);
	mIsStopped = false;
	mIsBreak = false;
}


bool
AosUdpTrafficGen::start()
{
	// 
	// Create the socket
	//
	mSock = OmnNew OmnUdp("Test", mLocalAddr, mLocalPort);
	OmnString errmsg;
	if (!mSock->connect(errmsg))
	{
		OmnAlarm << "Failed to connect the sender sock: " << errmsg << enderr;
		return false;
	}

	OmnThreadedObjPtr thisPtr(this, false);
	mSendThread = OmnNew OmnThread(thisPtr, "SendThread", 0, true, true);
	mRecvThread = OmnNew OmnThread(thisPtr, "RecvThread", 1, true, true);
	mSendThread->start();
	mRecvThread->start();
	mIsStopped = false;
	
	return true;
}

bool AosUdpTrafficGen::stop()
{
	if (mSendThread)
	{
		mSendThread->stop();
	}
	mIsStopped = true;
	return true;
}

bool AosUdpTrafficGen::isSendingStopped()
{
	return mIsStopped;
}

bool 
AosUdpTrafficGen::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == 0)
	{
		sendThreadFunc(state, thread);
	}
	else if (thread->getLogicId() == 1)
	{
		recvThreadFunc(state, thread);
	}
	else
	{
		OmnAlarm << "Unrecognized thread ID: " << (int)thread->getThreadId() << enderr;
		return false;
	}

	return true;
}


bool
AosUdpTrafficGen::sendThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	char* sendBuff=NULL;
	bool readFromFile = false;
	if (mFileName != "")
	{
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile->openFile(OmnFile::eReadOnly))
		{
			OmnAlarm << "Failed to open the file: " << mFileName << enderr;
			return false;
		}
		readFromFile = true;
	}
	else
	{
		sendBuff = OmnNew char[mPacketSize+1+MOD_VALUE];	
	}

	int pos = 0;
	int readsize = mPacketSize;
	int sleepTime=10;
	if (mSendRate >= 100)
	{
		sleepTime = 10;
	}
	else
	{
		sleepTime = 1000/mSendRate;
	}
	unsigned int rate = mSendRate*sleepTime/1000;// rate per 10ms
	if (rate == 0)
	{
		rate = 1;
	}
	

	char buff[1025];
	time_t beginTime=time(NULL);
	int lastTime=0;
	while (state == OmnThrdStatus::eActive)
	{
		static unsigned int initialValue = 0;
		static unsigned int currentValue = 0;
		if (readFromFile)
		{
			while (1)
			{
				int bytesRead = mFile->readToBuff(pos, readsize, buff);

				if (bytesRead == 0)
				{
					pos = 0;
					continue;
				}
	
				mSock->writeTo(buff, bytesRead, mRemoteAddr, mRemotePort);
				OmnTrace<<"Sent: "<<bytesRead<<endl;
				mPacketSent++;
				mBytesSent += bytesRead;
				currentValue ++;
	
				if (bytesRead < readsize)
				{
					pos = 0;
					continue;
				}

				pos += bytesRead;
				if (currentValue < initialValue)
				{
					currentValue = 0;
					initialValue = 0;
				}
				if (currentValue - initialValue >= rate)
				{
					OmnMsSleep(sleepTime);
					initialValue = currentValue;	
				}
			}
		}
		else
		{
				static int packetOrder=-1;
				int pt = mPacketSize + packetOrder%MOD_VALUE;
				mSock->writeTo(sendBuff, pt, mRemoteAddr, mRemotePort);
				OmnTrace<<"Sent: "<<mPacketSize<<endl;
				mPacketSent++;
				mBytesSent += mPacketSize;
				currentValue ++;
				if (currentValue < initialValue)
				{
					currentValue = 0;
					initialValue = 0;
				}
				if (currentValue - initialValue >= rate)
				{
		//		cout<<currentValue<<endl;
		//		cout<<rate<<endl;
					OmnMsSleep(sleepTime);
		//			cout<<"time out"<<endl;
		//			pthread_mutex_lock(&mMutex);
		//			pthread_cond_wait(&mCond,&mMutex);
		//			pthread_mutex_unlock(&mMutex);
					initialValue = currentValue;	
				}
		}
		if (mMaxPackets != 0)
		{
			if (mPacketSent >= mMaxPackets)
				stop();
		}
		if (mBreakTime > 0)
		{
			time_t curTime = time(NULL);
			lastTime = curTime - beginTime;
			//printf("beginTime=%d, curTime=%d, lastTime=%d\n", beginTime, curTime, lastTime);
			if (lastTime >= mBreakTime)
			{
				mIsBreak = true;	
				//printf("stop sending packets after %d second, press enter key to continue\n", lastTime);
				lastTime = 0;
				getchar();
				beginTime = time(NULL);
				mIsBreak = false;
				pthread_mutex_lock(&mMutex);
				pthread_cond_signal(&mCond);
				pthread_mutex_unlock(&mMutex);
			}
		}

	}
	
	return true;
}


bool
AosUdpTrafficGen::recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnConnBuffPtr buff;
	bool timeout = false;
	while (state == OmnThrdStatus::eActive)
	{
		mSock->readFrom(buff, 10, 0, timeout);
		if (timeout)
		{
			continue;
		}

		if (!buff)
		{
			OmnAlarm << "Failed to read" << enderr;
			continue;
		}

		mPacketReceived++;
		mBytesReceived += buff->getDataLength();

		OmnTrace << "Received: " << buff->getDataLength() << endl;
	}
	
	return true;
}


bool 
AosUdpTrafficGen::signal(const int threadLogicId)
{
	return true;
}


bool 
AosUdpTrafficGen::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}

unsigned int AosUdpTrafficGen::getPacketSent()
{
	return mPacketSent;
}

unsigned int AosUdpTrafficGen::getPacketReceived()
{
	return mPacketReceived;
}

unsigned int AosUdpTrafficGen::getBytesSent()
{
	return mBytesSent;
}

unsigned int AosUdpTrafficGen::getBytesReceived()
{
	return mBytesReceived;
}

bool AosUdpTrafficGen::getIsBreak()
{
	return mIsBreak;
}
