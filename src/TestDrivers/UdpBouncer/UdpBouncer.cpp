////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpBouncer.cpp
// Description:
//   
//
// Modification History:
// 07/12/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "UdpBouncer.h"

#include "Util/OmnNew.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"

AosUdpBouncer::AosUdpBouncer(
		const OmnIpAddr &localAddr, 
		const int localPort)
:
mLocalAddr(localAddr),
mLocalPort(localPort),
mRemotePort(0)
{
}


AosUdpBouncer::~AosUdpBouncer()
{
}


bool
AosUdpBouncer::start()
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
	mRecvThread = OmnNew OmnThread(thisPtr, "RecvThread", 1, true, true);

OmnTrace << "Receiver Thread ID: " << mRecvThread->getThreadId() << endl;

	mRecvThread->start();
	
	return true;
}


bool 
AosUdpBouncer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == 1)
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
AosUdpBouncer::recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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
		
		mSock->writeTo(buff->getData(),buff->getDataLength(),mRemoteAddr,mRemotePort);
	}
	
	return true;
}


bool 
AosUdpBouncer::signal(const int threadLogicId)
{
	return true;
}


bool 
AosUdpBouncer::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}



