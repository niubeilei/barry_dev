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
// 03/10/2012 Copied from UdpCommNew.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm1/UdpXmlComm.h"

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
#include "UtilComm/CommOwner.h"
#include "UtilComm1/UdpXmlCommListener.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static int sgReadingTimer = 30;


AosUdpXmlComm::AosUdpXmlComm(
		//const AosCommOwnerPtr &owner,
		const OmnString &localAddr, 
		const int port,
		const OmnString &name)
:
mLock(OmnNew OmnMutex()),
mName(name),
mLocalAddr(OmnIpAddr(localAddr)),
mLocalPort(port),
mSeqno(0)
//mOwner(owner)
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


AosUdpXmlComm::~AosUdpXmlComm()
{
}


bool
AosUdpXmlComm::sendTo(
		const AosXmlTagPtr &msg,
		const OmnIpAddr &remoteIpAddr,
		const int remotePort)
{
	aos_assert_r(msg, false);
	aos_assert_r(remotePort > 0, false);
	aos_assert_r(remoteIpAddr.isValid(), false);
	aos_assert_r(mUdpConn, false);
	mLock->lock();
	msg->setAttr(AOSTAG_UDPCOMM_SEQNO, mSeqno++);
	OmnString str = msg->toString();
	bool rslt = mUdpConn->writeTo(str.data(), str.length(), remoteIpAddr, remotePort);
	mLock->unlock();
	return rslt;
}


OmnString
AosUdpXmlComm::toString() const
{
	OmnString str = "UDP Comm: ";
	str << "Name=" << mName
		<< ", Local Addr: " << mLocalAddr.toString()
		<< ", Local Port: " << mLocalPort;
	return str;
}


bool		
AosUdpXmlComm::isConnGood() const
{
	aos_assert_r(mUdpConn, false);
	return mUdpConn->isConnGood();
}


bool
AosUdpXmlComm::closeConn()
{
	aos_assert_r(mUdpConn, false);
	return mUdpConn->closeConn();
}


bool
AosUdpXmlComm::connect(OmnString &err)
{
	aos_assert_r(mUdpConn, false);
	OmnRslt rslt = mUdpConn->connect(err);
	mLocalPort = mUdpConn->getLocalPort();
	return rslt;
}


bool
AosUdpXmlComm::reconnect(OmnString &err)
{
	aos_assert_r(mUdpConn, false);
	return mUdpConn->reconnect(err);
}


bool
AosUdpXmlComm::readFrom(
		AosXmlTagPtr &msg,
		const int timerSec,
		const int timeruSec,
		bool &isTimeout, 
		OmnIpAddr &remote_addr, 
		int &remote_port)
{
	aos_assert_r(mUdpConn, false);
	OmnConnBuffPtr buffer;
	OmnRslt rslt = mUdpConn->readFrom(buffer, timerSec, timeruSec, isTimeout);
	if (!rslt || !buffer) return false;
	
	try
	{
		msg = AosXmlParser::parseStatic(buffer AosMemoryCheckerArgs);
		if (!msg) return false;
		remote_addr = buffer->getRemoteAddr();
		remote_port = buffer->getRemotePort();
		return true;
	}

	catch (...)
	{
		OmnAlarm << "Failed parsing the XML: " << buffer->getData() << enderr;
		return false;
	}

	return true;
}


bool 
AosUdpXmlComm::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTraceThread << "Enter AosUdpXmlCommReadThreadFunc" << endl;

	AosXmlTagPtr msg;
	OmnRslt rslt;
	bool isTimeout = false;
	OmnIpAddr remote_addr;
	int remote_port;
	AosUdpXmlCommPtr thisptr(this, false);
	while (state == OmnThrdStatus::eActive)
	{
		rslt = readFrom(msg, sgReadingTimer, 0, isTimeout, remote_addr, remote_port);
		if (isTimeout)
		{
			// It is timeout
			continue;
		}

		if (!rslt || !msg)
		{
			// Failed to read
			// Currently we wait for eReadFailIntervalTimerSec seconds
			// and then try it again.
			OmnSleep(eReadFailIntervalTimerSec);
			continue;
		}

		// It read successfully.
		if (mListener.isNull())
		{
			// This should not happen
			OmnAlarm << "No requester" << enderr;
			continue;
		}
		
		mListener->msgRead(thisptr, msg, remote_addr, remote_port);
	}

	OmnTrace << "Leave OmnOmnPhoneMsgProc::threadFunc" << endl;
	return true;
}


bool 
AosUdpXmlComm::signal(const int threadLogicId)
{
	// Don't need to do anything
	return true;
}


bool
AosUdpXmlComm::startReading(const AosUdpXmlCommListenerPtr &callback)
{
	// There are two methods to read messages from a comm. One is to call
	// the member function: readFrom(...) directly and the other is to 
	// use this function. 
	//
	// This function checks whether a reading thread has been created. If not,
	// it create one. If the thread is already created, it starts the thread.
	if (!mListener.isNull())
	{
		// This may be a problem.
		OmnAlarm << "Found start reading override! The old requester: " 
			<< mListener->getListenerName()
			<< " and the new Requester: " 
			<< callback->getListenerName() 
			<< ", " << mName << enderr;
	}

	mListener = callback;

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
AosUdpXmlComm::stopReading(const AosUdpXmlCommListenerPtr &requester)
{
	if (mReadingThread.isNull())
	{
		// There is no reading thread.
		return true;
	}

	if (mListener != requester)
	{
		OmnAlarm << "To stop a thread that is not owned by the requester" << enderr;
		return false;
	}

	mListener = 0;
	return mReadingThread->stop();
}


bool
AosUdpXmlComm::forceStop()
{
	// Call this function only when the system wants to exit
	// This should be called from OmnCommMgr.
	if (mReadingThread.isNull())
    {
        // There is no reading thread.
        return true;
    }

    return mReadingThread->stop();

	mListener = 0;
}


bool
AosUdpXmlComm::checkThread(OmnString &, const int tid) const
{
	// Since it is not managed by ThreadMgr, we always return true.
    return true;
}


int
AosUdpXmlComm::getSock() const
{
	if (mUdpConn.isNull())
	{
		return -1;
	}

	return mUdpConn->getSock();
}

