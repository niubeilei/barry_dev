////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCommClt.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpCommClt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "Message/Msg.h"
#include "Network/Network.h"
#include "Network/NetIf.h"
#include "Porting/Sleep.h"
#include "Porting/GetErrnoStr.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Util/SerialTo.h"
#include "Util/OmnNew.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpClientSingleThrd.h"
#include "UtilComm/CommTypes.h"
#include "XmlUtil/XmlItemName.h"
#include "XmlParser/XmlItem.h"



OmnTcpCommClt::OmnTcpCommClt(
	const OmnIpAddr &remoteAddr, 
	const int remotePort,
	const int num_ports, 
	const AosTcpLengthType length_type, 
	const OmnString &fname, 
	const int line, 
	const bool single_flag)
:
OmnComm(OmnCommProt::eTcpClient, ""),
mIsReading(false),
mIsSmartReading(false),
mLock(OmnNew OmnMutex()),
mRemoteAddr(remoteAddr), 
mRemotePort(remotePort), 
mNumPorts(num_ports) ,
mLengthType(length_type) ,
mFname(fname),
mLine(line)
{
	if (!remoteAddr.isValid())
	{
		OmnAlarm << "Remote address incorrect: " << remoteAddr.toString()
			<< ":" << remotePort << enderr;
	}

	if (remotePort <= 0)
	{
		OmnAlarm << "Remote port incorrect: " << remotePort << enderr;
	}

	if (!single_flag)
	{
		mConn = OmnNew OmnTcpClient("", remoteAddr, remotePort, num_ports, length_type);
	}
	else
	{
		mConn = OmnNew OmnTcpClientSingleThrd("", remoteAddr, remotePort, num_ports, length_type);
	}
	OmnString err;
	//mConn->connect(err);
}


OmnTcpCommClt::OmnTcpCommClt(
	const OmnString &path, 
	const OmnString &sfname,
	const AosTcpLengthType length_type, 
	const OmnString &fname, 
	const int line,
	const bool single_flag)
:
OmnComm(OmnCommProt::eTcpClient, ""),
mIsReading(false),
mIsSmartReading(false),
mLock(OmnNew OmnMutex()),
mRemoteAddr("0"), 
mRemotePort(0), 
mNumPorts(0) ,
mLengthType(length_type) ,
mFname(fname),
mLine(line)
{
	if (!single_flag)
	{
		mConn = OmnNew OmnTcpClient(path, sfname, length_type);
	}
	else
	{
		mConn = OmnNew OmnTcpClientSingleThrd(path, sfname, length_type);
	}
	OmnString err;
	//mConn->connect(err);
}


OmnTcpCommClt::OmnTcpCommClt(
		const OmnCommProt::E protocol, 
		const OmnXmlItemPtr &def,
        const int targetId,
        const OmnNetEtyType::E targetType,
		const OmnCommGroupPtr &comm, 
		const OmnString &fname, 
		const int line,
		const bool single_flag)
:
OmnComm(protocol, false, comm, targetId, targetType),
mIsReading(false),
mIsSmartReading(false),
mLock(OmnNew OmnMutex()),
mFname(fname),
mLine(line)
{
	// Connect
	OmnString err;
	if (!mConn->connect(err))
	{
		// It is okey that the connection cannot be created for now. 
		// The class will keep on trying.
	}
}


OmnTcpCommClt::~OmnTcpCommClt()
{
	if (mThread)	//fix coredump by White 2015-08-21 09:06:01
	{
		mThread->stop();
		OmnThreadMgr::getSelf()->removeThread(mThread);
		mThread = 0;
	}
}


// OmnRslt
// OmnTcpCommClt::config(const OmnXmlItemPtr &def)
// {
// }


bool
OmnTcpCommClt::reconnect()
{
	// Any time when the connection is not good, this function is called.
	// It keeps on trying to connect. The function will not return until
	// it connects. 
	if (mRequester) mRequester->readingFailed();
	mLock->lock();
	while (1)
	{
		// Chen Ding, 04/07/2012
		// if (mConn->isConnGood())
		// {
		// 	mLock->unlock();
		// 	return true;
		// }
		OmnString err;
		mConn->closeConn();
		if (!mConn->connect(err))
		{
			OmnSleep(OmnCommTypes::eReadFailIntervalTimerSec);
			continue;
		}
		break;
	}
	mLock->unlock();
	return true;
}


OmnRslt
OmnTcpCommClt::sendTo(const char *data, const int length)
{
	// It sends the data 'data'. If connection is not good, it will
	// try to reconnect. The function will not return until it 
	// reconnects.
    OmnRslt rslt = false;
	while(!rslt)
	{
		rslt = mConn->smartSend(data, length);
		if (!rslt)
		{
			reconnect();
		}
	}
	return true;
}


// Chen Ding, 04/07/2012
bool
OmnTcpCommClt::smartSend(const char *data1, const int len1, const char *data2, const int len2)
{
	// It sends the data 'data'. If connection is not good, it will
	// try to reconnect. The function will not return until it 
	// reconnects.
    OmnRslt rslt = false;
	while (!rslt)
	{
		rslt = mConn->smartSend(data1, len1, data2, len2);
		if (!rslt)
		{
			reconnect();
		}
	}
	return true;
}


OmnString
OmnTcpCommClt::toString() const
{
	if (!mConn) return "";
    if (mConn.isNull())
    {
        return OmnString("TCP Single Port Communicator: ")
			   << mName << ". Connection is null!";
    }
    else
    {
		OmnString ss = "TcpCommClt: ";
        // return OmnString("TCP Single PortCommunicator: ") << mName;
		ss << mConn->toString();
		return ss;
    }
}


bool
OmnTcpCommClt::isConnGood() const
{
    if (!mConn.isNull() && mConn->isConnGood())
    {
        return true;
    }

    return false;
}


OmnRslt
OmnTcpCommClt::closeConn()
{
	aos_assert_r(mConn, false);
    return mConn->closeConn();
}


OmnRslt
OmnTcpCommClt::connect(OmnString &err)
{
	aos_assert_r(mConn, false);
    return mConn->connect(err);
}


bool
OmnTcpCommClt::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTraceThread << "Enter OmnTcpCommClt: " << mName << endl;

    OmnConnBuffPtr buff;
    OmnRslt rslt;
    while (state == OmnThrdStatus::eActive)
    {
		if(mConn->isConnGood())
		{
			rslt = mConn->smartRead(buff);
    	    if (!rslt)
       		{
           	 	// Failed to read
            	// Currently we wait for eReadFailIntervalTimerSec seconds
            	// and then try it again.
            	//
				// We do not tread it as an alarm. Chen Ding, 02/02/2012
				// OmnAlarm << "Failed to read: " << rslt.toString() << enderr;
				reconnect();
            	continue;
        	}


			OmnTraceComm << "Read: " << buff->getDataLength() << endl;
        	// It read successfully.
        	if (!mRequester)
        	{
            	// This should not happen
            	OmnAlarm << OmnErrId::eAlarmProgramError
					<< "mRequest is null" << enderr;
            	continue;
        	}
        	else
        	{
				if(buff)
				{
					buff->setConn(mConn);
				}
            	mRequester->msgRead(buff);
        	}
    	}
		else
		{
			reconnect();
		}
	}

    OmnTrace << "Leave OmnOmnPhoneMsgProc::threadFunc" << endl;
   	return true;
}


bool
OmnTcpCommClt::signal(const int threadLogicId)
{
    // Don't need to do anything
    return true;
}


OmnRslt
OmnTcpCommClt::startReading(const OmnCommListenerPtr &callback)
{
    //
    // There are two methods to read messages from a comm. One is to call
    // the member function: readFrom(...) directly and the other is to
    // use this function.
    //
    // This function checks whether a reading thread has been created. If not,
    // it create one. If the thread is already created, it starts the thread.
    //

	// Chen Ding, 09/29/2003
    if (!mRequester.isNull() && mRequester != callback)
    {
        // This may be a problem.
        OmnAlarm << OmnErrId::eAlarmProgramError
            << "Found start reading override! The old requester: "
            << mRequester->getCommListenerName()
            << " and the new Requester: " << callback->getCommListenerName() << enderr;
    }

    mRequester = callback;

	// Chen Ding, 09/28/2003
	mIsReading = true;

    OmnTraceComm << callback->getCommListenerName()
        << " start reading on: "
        << mConn->getLocalIpAddr().toString()
        << ":" << mConn->getLocalPort() << endl;

    if (mThread.isNull())
    {
        //
        // The thread has not been created yet.
        //
		OmnThreadedObjPtr thisPtr(this, false);
        mThread = OmnNew OmnThread(thisPtr, mName, 0, false, true, __FILE__, __LINE__);
    }

    if (mThread->getStatus() != OmnThrdStatus::eActive)
    {
        mThread->start();
    }
	else
	{
		// Chen Ding, 09/29/2003
		OmnTrace << "+++++ Thread is already active" << endl;
	}

    return true;
}


OmnRslt
OmnTcpCommClt::stopReading(const OmnCommListenerPtr &requester)
{
    if (mThread.isNull())
    {
        //
        // There is no reading thread.
        //
        return true;
    }

	//
	// Chen Ding, 09/28/2003
	//
	if (!mIsReading)
	{
		//
		// It is not reading. That's ok. Just return.
		//
		return true;
	}

	if (mRequester.isNull())
	{
		return OmnAlarm << OmnErrId::eAlarmProgramError
			<< "To stop reading, but no mRequest!" << enderr;
	}

    if (mRequester != requester)
    {
		// 
		// Chen Ding, 09/28/2003
		//
        return OmnAlarm << OmnErrId::eAlarmProgramError
			<< "To stop a thread that is not owned by the requester"
            << enderr;
    }

	mIsReading = false;
    mRequester = 0;
    mThread->stop();
	// OmnThreadMgr::getSelf()->removeThread(mThread);
	// mThread = 0;
	return true;
}


OmnRslt
OmnTcpCommClt::forceStop()
{
    //
    // Call this function only when the system wants to exit
    // This should be called from OmnCommMgr.
    //
    if (mThread.isNull())
    {
        //
        // There is no reading thread.
        //
        return true;
    }

	//
	// Chen Ding, 09/28/2003
	// This is a bug.
	//
    // return mThread->stop();
    mThread->stop();

    mRequester = 0;
	mIsReading = false;

	return true;
}


OmnRslt     
OmnTcpCommClt::sendTo(const char *data,
                      const int length,
                      const OmnIpAddr &recvIpAddr,
                      const int recvPort)
{
	// The function sends the data 'data'. If the connection is broken,
	// it will try to reconnect. The function will not return until
	// it reconnects and sends.
	// Chen Ding, 11/16/2010
    // return mConn->writeTo(data, length, recvIpAddr, recvPort);
    OmnRslt rslt = false;
	while(!rslt)
	{
		rslt = mConn->smartSend(data, length);
		if (!rslt)
		{
			reconnect();
		}
	}
	return true;
}


OmnRslt
OmnTcpCommClt::reconnect(OmnString &err)
{
	err = "Not implemented yet";
	return OmnAlarm << OmnErrId::eAlarmProgramError
		<< "Not implemented yet" << enderr;
}


void
OmnTcpCommClt::heartbeat()
{
	// Currently not managed by ThreadMgr. Do nothing
}


// E007, 07/13/2003, 2003-0073
bool
OmnTcpCommClt::checkThread(OmnString &, const int tid) const
{
	//
	// Currently not managed by ThreadMgr. Always return true
	//
    return true;
}


OmnIpAddr
OmnTcpCommClt::getLocalIpAddr() const
{
	if (mConn.isNull())
	{
		OmnAlarm << OmnErrId::eWarnCommError
			<< "TcpCommClt: mConn is null!" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return mConn->getLocalIpAddr();
}


int
OmnTcpCommClt::getLocalPort() const
{
	if (mConn.isNull())
	{
		OmnAlarm << OmnErrId::eWarnCommError
			<< "TcpCommClt: mConn is null!" << enderr;
		return -1;
	}

	return mConn->getLocalPort();
}


int
OmnTcpCommClt::getSock() const
{
	if (mConn.isNull())
	{
		return -1;
	}

	return mConn->getSock();
}


bool	
OmnTcpCommClt::smartSend(const char *data, const int len)
{
	aos_assert_r(mConn, false);
	return mConn->smartSend(data, len);
}


bool	
OmnTcpCommClt::startSmartReading(const OmnCommListenerPtr &callback)
{
	mIsSmartReading = true;
	return startReading(callback);
}


bool
OmnTcpCommClt::destroyConn()
{
	closeConn();
	stopReading(0);
	if (mThread)
	{
		OmnThreadMgr::getSelf()->removeThread(mThread);
	}
	return true;
}

