////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastFtpSender.cpp
// Description:
//   
//
// Modification History:
// 01/03/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "FastFTP/Sender/FastFtpSender.h"

#include "Util/OmnNew.h"
#include "UtilComm/Udp.h"
#include "Alarm/Alarm.h"
#include "Thread/Thread.h"
#include "Porting/Sleep.h"

AosFastFtpSender::AosFastFtpSender(
		const OmnString &filename, 
		const OmnIpAddr &localAddr, 
		const int localPort,
		const OmnIpAddr &remoteAddr, 
		const int remotePort)
:
mFileName(filename),
mLocalAddr(localAddr),
mLocalPort(localPort),
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mSendingFinished(false)
{
}


bool
AosFastFtpSender::start()
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
	mSendThread->start();
	mRecvThread->start();
	
	return true;
}


bool 
AosFastFtpSender::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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
AosFastFtpSender::sendThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
	if (!mFile->openFile(OmnFile::eReadOnly))
	{
		OmnAlarm << "Failed to open the file: " << mFileName << enderr;
		return false;
	}

	int pos = 0;
	int readsize = 1024;
	char buff[1025];
	while (state == OmnThrdStatus::eActive)
	{
		while (1)
		{
			int bytesRead = mFile->readToBuff(pos, readsize, buff);

			if (bytesRead == 0)
			{
				// 
				// Finished
				//
				mSendingFinished = true;
				while (1)
				{
					OmnSleep(1);
				}
				return true;
			}

			mSock->writeTo(buff, bytesRead, mRemoteAddr, mRemotePort);

			if (bytesRead < readsize)
			{
				// 
				// Finished reading
				//
				mSendingFinished = true;
				while (1)
				{
					OmnSleep(1);
				}

				return true;
			}

			pos += bytesRead;
		}
	}
	
	return true;
}


bool
AosFastFtpSender::recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1);
	}
	
	return true;
}


bool 
AosFastFtpSender::signal(const int threadLogicId)
{
	return true;
}


bool 
AosFastFtpSender::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}



