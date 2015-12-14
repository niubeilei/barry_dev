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
//
// Modification History:
// 03/10/2012 Copied from UdpComm.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm1/UdpCommNew.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Debug/Error.h"
#include "Message/Msg.h"
#include "Message/Ptrs.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommListener.h"


static int sgReadingTimer = 30;


AosUdpCommNew::AosUdpCommNew(
		const OmnString &localAddr, 
		const int port,
		const OmnString &name)
:
mName(name),
mLocalAddr(OmnIpAddr(localAddr)),
mLocalPort(port)
{
	// Make sure the IP address and port are correct.
	if (!mLocalAddr.isValid())
	{
		OmnAlarm << "Incorrect local address: " << localAddr.toString() 
			<< " for " << name << enderr;
		OmnThrowException("Incorrect local address");
		return;
	}

	if (mLocalPort < 0)
	{
		OmnAlarm << "Incorrect local port: " << mLocalPort 
			<< " for: " << name << enderr;
		OmnThrowException("Incorrect local port");
		return;
	}

	mUdpConn = OmnNew OmnUdp(mName, mLocalAddr, mLocalPort);

	OmnString err;
	OmnRslt rslt = mUdpConn->connect(err);
	if (!rslt)
	{
		OmnThrowException(err);
		return;
	}

	mLocalPort = mUdpConn->getLocalPort();
}


AosUdpCommNew::~AosUdpCommNew()
{
}


bool
AosUdpCommNew::sendTo(
		const char *data, 
		const int length,
		const OmnIpAddr &remoteIpAddr,
		const int remotePort)
{
	aos_assert_r(data, false);
	aos_assert_r(remotePort > 0, false);
	aos_assert_r(remoteIpAddr.isValid(), false);
	aos_assert_r(mUdpConn, false);
	return mUdpConn->writeTo(data, length, remoteIpAddr, remotePort);
}


OmnString
AosUdpCommNew::toString() const
{
	OmnString str = "UDP Comm: ";
	str << "Name=" << mName
		<< ", Local Addr: " << mLocalAddr.toString()
		<< ", Local Port: " << mLocalPort;
	return str;
}


bool		
AosUdpCommNew::isConnGood() const
{
	aos_assert_r(mUdpConn, false);
	return mUdpConn->isConnGood();
}


bool
AosUdpCommNew::closeConn()
{
	aos_assert_r(mUdpConn, false);
	return mUdpConn->closeConn();
}


bool
AosUdpCommNew::connect(OmnString &err)
{
	aos_assert_r(mUdpConn, false);
	OmnRslt rslt = mUdpConn->connect(err);
	mLocalPort = mUdpConn->getLocalPort();
	return rslt;
}


bool
AosUdpCommNew::reconnect(OmnString &err)
{
	aos_assert_r(mUdpConn, false);
	return mUdpConn->reconnect(err);
}


bool
AosUdpCommNew::readFrom(
		OmnConnBuffPtr &buffer,
		const int timerSec,
		const int timeruSec,
		bool &isTimeout)
{
	// It reads from the connection. If successful, it returns the buffer. 
	// Otherwise, it returns error. Note that if it times out, it will not
	// return a null buffer and 'isTimeout' is set to true. 
	aos_assert_r(mUdpConn, false);
	OmnRslt rslt = mUdpConn->readFrom(buffer, timerSec, timeruSec, isTimeout);
	return rslt;
}


bool 
AosUdpCommNew::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTraceThread << "Enter AosUdpCommNewReadThreadFunc" << endl;

	OmnConnBuffPtr buff;
	OmnRslt rslt;
	bool isTimeout = false;
	while (state == OmnThrdStatus::eActive)
	{
		rslt = readFrom(buff, sgReadingTimer, 0, isTimeout);
		if (isTimeout)
		{
			// It is timeout
			continue;
		}

		if (!rslt)
		{
			// Failed to read
			// Currently we wait for eReadFailIntervalTimerSec seconds
			// and then try it again.
			OmnSleep(eReadFailIntervalTimerSec);
			continue;
		}

		// It read successfully.
		if (mRequester.isNull())
		{
			// This should not happen
			OmnAlarm << "No requester" << enderr;
			continue;
		}
		
		mRequester->msgRead(buff);
	}

	OmnTrace << "Leave OmnOmnPhoneMsgProc::threadFunc" << endl;
	return true;
}


bool 
AosUdpCommNew::signal(const int threadLogicId)
{
	// Don't need to do anything
	return true;
}


bool
AosUdpCommNew::startReading(const OmnCommListenerPtr &callback)
{
	// There are two methods to read messages from a comm. One is to call
	// the member function: readFrom(...) directly and the other is to 
	// use this function. 
	//
	// This function checks whether a reading thread has been created. If not,
	// it create one. If the thread is already created, it starts the thread.
	if (!mRequester.isNull())
	{
		//
		// This may be a problem.
		//
		OmnAlarm << "Found start reading override! The old requester: " 
			<< mRequester->getCommListenerName()
			<< " and the new Requester: " 
			<< callback->getCommListenerName() 
			<< ", " << mName << enderr;
	}

	mRequester = callback;

	OmnTrace << callback->getCommListenerName()
		<< " start reading on: " 
		<< mLocalAddr.toString() 
		<< ":" << mLocalPort << endl;

	if (mReadingThread.isNull())
	{
		// The thread has not been created yet.
		OmnThreadedObjPtr thisPtr(this, false);
		mReadingThread = OmnNew OmnThread(thisPtr, mName, 0, false, true);
	}

	if (mReadingThread->getStatus() != OmnThrdStatus::eActive)
	{
		mReadingThread->start();
	}

	return true;
}


bool
AosUdpCommNew::stopReading(const OmnCommListenerPtr &requester)
{
	if (mReadingThread.isNull())
	{
		// There is no reading thread.
		return true;
	}

	if (mRequester != requester)
	{
		OmnAlarm << "To stop a thread that is not owned by the requester" << enderr;
		return false;
	}

	mRequester = 0;
	return mReadingThread->stop();
}


bool
AosUdpCommNew::forceStop()
{
	// Call this function only when the system wants to exit
	// This should be called from OmnCommMgr.
	if (mReadingThread.isNull())
    {
        // There is no reading thread.
        return true;
    }

    return mReadingThread->stop();

	mRequester = 0;
}


bool
AosUdpCommNew::checkThread(OmnString &, const int tid) const
{
	// Since it is not managed by ThreadMgr, we always return true.
    return true;
}


int
AosUdpCommNew::getSock() const
{
	if (mUdpConn.isNull())
	{
		return -1;
	}

	return mUdpConn->getSock();
}

