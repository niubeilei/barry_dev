////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastFtpReceiver.cpp
// Description:
//   
//
// Modification History:
// 01/03/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "FastFTP/Receiver/FastFtpReceiver.h"

#include "Util/OmnNew.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"
#include "Alarm/Alarm.h"
#include "Thread/Thread.h"
#include "Porting/Sleep.h"

AosFastFtpReceiver::AosFastFtpReceiver(
		const OmnString &filename, 
		const OmnIpAddr &localAddr, 
		const int localPort)
:
mFileName(filename),
mLocalAddr(localAddr),
mLocalPort(localPort),
mRemotePort(0)
{
}


AosFastFtpReceiver::~AosFastFtpReceiver()
{
}


bool
AosFastFtpReceiver::start()
{
	// 
	// Create the socket
	//
	mSock = OmnNew OmnUdp("Send", mLocalAddr, mLocalPort);
	OmnString errmsg;
	if (!mSock->connect(errmsg))
	{
		OmnAlarm << "Failed to connect the sender sock: " << errmsg << enderr;
		return false;
	}

	OmnThreadedObjPtr thisPtr(this, false);
	mSendThread = OmnNew OmnThread(thisPtr, "SendThread", 0, true, true, __FILE__, __LINE__);
	mRecvThread = OmnNew OmnThread(thisPtr, "RecvThread", 1, true, true, __FILE__, __LINE__);

OmnTrace << "Sender Thread ID: " << mSendThread->getThreadId() << endl;
OmnTrace << "Receiver Thread ID: " << mRecvThread->getThreadId() << endl;

	mSendThread->start();
	mRecvThread->start();
	
	return true;
}


bool 
AosFastFtpReceiver::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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
AosFastFtpReceiver::sendThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		while (1)
		{
			/*
			int bytesRead = mFile->readToBuff(pos, readsize, buff);

			if (bytesRead == 0)
			{
				// 
				// Finished
				//
				return true;
			}

			mSock->writeTo(buff, bytesRead, mRemoteAddr, mRemotePort);

			if (bytesRead < readsize)
			{
				// 
				// Finished reading
				//
				return true;
			}
			*/

			OmnSleep(1);
		}
	}
	
	return true;
}


bool
AosFastFtpReceiver::recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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

		mRemoteAddr = buff->getRemoteAddr();
		mRemotePort = buff->getRemotePort();

		OmnTrace << "Received: " << buff->getDataLength() << endl;
	}
	
	return true;
}


bool 
AosFastFtpReceiver::signal(const int threadLogicId)
{
	return true;
}


bool 
AosFastFtpReceiver::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}



