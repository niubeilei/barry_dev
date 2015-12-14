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
// 2013/06/15 Created based on UdpComm.cpp Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/UdpCommNew.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Debug/Error.h"
#include "Message/Msg.h"
#include "Message/Ptrs.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Thread/Thread.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "UtilComm/Udp.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/CommGroup.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"


static int sgReadingTimer = 1;


AosUdpComm::AosUdpComm(
		const AosRundataPtr &rdata, 
		const OmnString &name,
		const OmnIpAddr &localAddr,
		const int localPort)
:
mLocalIpAddr(localAddr),
mLocalPort(localPort)
{
	mName = name;
	try
	{
		mUdpConn = OmnNew OmnUdp(mName, mLocalIpAddr, mLocalPort);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating connection" << enderr;
		return;
	}

	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);

	OmnString err;
	OmnRslt rslt = mUdpConn->connect(err);
	if (!rslt)
	{
		OmnAlarm << "Failed connecting: " << err << enderr;
		return;
	}

	mLocalPort = mUdpConn->getLocalPort();
}


AosUdpComm::~AosUdpComm()
{
}


bool
AosUdpComm::config(const AosXmlTagPtr &def)
{
	return false;
}


bool
AosUdpComm::sendTo(
		const AosRundataPtr &rdata, 
		const char *data, 
		const int length,
		const OmnIpAddr &remoteIpAddr,
		const int remotePort)
{
	aos_assert_rr(remotePort > 0, rdata, false);
	// if (remotePort <= 0)
	// {
	// 	aos_assert_rr(mRemotePort > 0, rdata, false);
	// 	return mUdpConn->writeTo(data, length, mRemoteIpAddr, mRemotePort);
	// }
	return mUdpConn->writeTo(data, length, remoteIpAddr, remotePort);
}


OmnString
AosUdpComm::toString() const
{
	if (mUdpConn.isNull())
	{
		return OmnString("UDP Comm: ")
			   << mName << ". Connection is null!";
	}
	else
	{
		return OmnString("UDP Comm: ")
			   << mName << ". UDP Connection Info: " 
			<< mUdpConn->toString();
	}
}


bool		
AosUdpComm::isConnGood() const
{
	if (!mUdpConn.isNull() && mUdpConn->isConnGood())
	{
		return true;
	}

	return false;
}


OmnRslt
AosUdpComm::closeConn()
{
	return mUdpConn->closeConn();
}


OmnRslt
AosUdpComm::connect(OmnString &err)
{
	OmnRslt rslt = mUdpConn->connect(err);
	mLocalPort = mUdpConn->getLocalPort();
	return rslt;
}


OmnRslt
AosUdpComm::reconnect(OmnString &err)
{
	return mUdpConn->reconnect(err);
}


bool
AosUdpComm::readFrom(
		const AosRundataPtr &rdata,
		OmnConnBuffPtr &buffer,
		const int timerSec,
		const int timeruSec,
		bool &isTimeout)
{
	// It reads from the connection. If successful, it returns the buffer. 
	// Otherwise, it returns error. Note that if it times out, it will not
	// return a null buffer and 'isTimeout' is set to true. 

	// Read from the connection
	OmnRslt rslt = mUdpConn->readFrom(buffer, timerSec, timeruSec, isTimeout);
	if (!rslt)
	{
		return rslt;
	}

	if (isTimeout)
	{
		return true;
	}

	OmnCheckAReturn(buffer, "Buffer is null", false);
	return true;
}


bool 
AosUdpComm::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTraceThread << "Enter AosUdpCommReadThreadFunc" << endl;

	OmnConnBuffPtr buff;
	bool rslt;
	bool isTimeout = false;
	while (state == OmnThrdStatus::eActive)
	{
		rslt = readFrom(mRundata, buff, sgReadingTimer, 0, isTimeout);
		mThreadStatus = true;
		if (isTimeout)
		{
			// It is timeout
			continue;
		}

		if (!rslt)
		{
			// Failed reading.
			// Currently we wait for eReadFailIntervalTimerSec seconds
			// and then try it again.
			OmnSleep(OmnCommTypes::eReadFailIntervalTimerSec);
			continue;
		}

		// It read successfully.
		if (mRequester.isNull())
		{
			//
			// This should not happen
			//
			OmnAlarm << "No requester" << enderr;
			continue;
		}

		mRequester->msgRead(buff);
	}

	OmnTrace << "Leave OmnOmnPhoneMsgProc::threadFunc" << endl;
	return true;
}


bool 
AosUdpComm::signal(const int threadLogicId)
{
	// Don't need to do anything
	mThreadStatus = false;
	return true;
}


OmnRslt
AosUdpComm::startReading(const OmnCommListenerPtr &callback)
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

	OmnScreen << callback->getCommListenerName()
		<< " start reading on: " 
		<< mLocalIpAddr.toString() 
		<< ":" << mLocalPort << endl;

	if (mReadingThread.isNull())
	{
		//
		// The thread has not been created yet.
		//
		OmnThreadedObjPtr thisPtr(this, false);
		mReadingThread = OmnNew OmnThread(thisPtr, mName, 0, false, true, __FILE__, __LINE__);
	}

	if (mReadingThread->getStatus() != OmnThrdStatus::eActive)
	{
		mReadingThread->start();
	}

	return true;
}


OmnRslt
AosUdpComm::stopReading(const OmnCommListenerPtr &requester)
{
	if (mReadingThread.isNull())
	{
		// There is no reading thread.
		return true;
	}

	if (mRequester != requester)
	{
		OmnAlarm << "To stop a thread that is not owned by the requester"
			<< enderr;
		return false;
	}

	mRequester = 0;
	return mReadingThread->stop();
}


OmnRslt
AosUdpComm::forceStop()
{
	if (mReadingThread.isNull())
    {
        // There is no reading thread.
        return true;
    }

    return mReadingThread->stop();
}


bool
AosUdpComm::checkThread(OmnString &, const int tid) const
{
	// Since it is not managed by ThreadMgr, we always return true.
    return mThreadStatus;
}


int
AosUdpComm::getSock() const
{
	if (mUdpConn.isNull()) return -1;
	return mUdpConn->getSock();
}

