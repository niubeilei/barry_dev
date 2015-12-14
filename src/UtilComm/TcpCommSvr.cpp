////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCommSvr.cpp
// Description:
//   
//
// Modification History:
// 		No one is using it and it was not defined very well. Comment
// 		out everything for now. Will re-work on it. CD 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpCommSvr.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "Message/Msg.h"
#include "Network/Network.h"
#include "Network/NetIf.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/SerialTo.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/CommTypes.h"
#include "XmlUtil/XmlItemName.h"
#include "XmlParser/XmlItem.h"




OmnTcpCommSvr::OmnTcpCommSvr(const OmnIpAddr &localAddr, 
							 const int localPort, 
							 const int num_ports,
							 const OmnString &name)
:
mName(name),
mLocalAddr(localAddr),
mLocalPort(localPort),
mNumLocalPorts(num_ports)
{
	//
	// Connect
	//
	mTcpServer = OmnNew OmnTcpServer(mLocalAddr, mLocalPort, mNumLocalPorts, mName, eAosTLT_FirstFourHigh); 

	OmnString err;
	if (!mTcpServer->connect(err))
	{
		//
		// This is no good. 
		//
		OmnExcept e(OmnFileLine, OmnErrId::eCommError, err);
		throw e;
	}

	//OmnThreadedObjPtr thisPtr(this, false);
	//mThread = OmnNew OmnThread(thisPtr, OmnString("TcpCommSvr:") 
	//				<< mTcpServer->getName(), 0, false);
}


OmnTcpCommSvr::~OmnTcpCommSvr()
{
}


OmnRslt
OmnTcpCommSvr::sendTo(const OmnMsgPtr &msg, const OmnTcpClientPtr &client)
{
	//
	// This function serializes 'msg' into OmnConnBuffer and then sends it
	// through its connection. If the connection is not good, it returns false.
	// 
	OmnSerialTo s;
    if (!msg->serializeTo(s))
    {
        return OmnAlarm << OmnErrId::eSerializeToError
			<< "Failed to serialize the message" << enderr;
    }

    OmnTraceComm << "Send a msg content: \n" << s.getData() << endl;
    return mTcpServer->writeTo((const char *)s.getData(), 
						  s.getDataLength(),
						  client->getSock());
}


OmnRslt
OmnTcpCommSvr::sendTo(const char *data, 
					  const int length,
					  const int sock)
{
    return mTcpServer->writeTo(data, length, sock);
}


OmnString
OmnTcpCommSvr::toString() const
{
    if (mTcpServer.isNull())
    {
        return OmnString("TCP Server Communicator: ")
			   << mName << ". Connection is null!";
    }
    else
    {
        return OmnString("TCP Server PortCommunicator: ") << mName;
    }
}


bool
OmnTcpCommSvr::isConnGood() const
{
	OmnNotImplementedYet;
	return true;
}


OmnRslt
OmnTcpCommSvr::closeConn()
{
    return mTcpServer->closeConn();
}


OmnRslt
OmnTcpCommSvr::connect(OmnString &err)
{
    return mTcpServer->connect(err);
}


OmnRslt
OmnTcpCommSvr::readFrom(OmnConnBuffPtr &buffer, OmnTcpClientPtr &client)
{
    //
    // It creates a OmnConnBuff and then asks the connection to read a
    // message. If successful, it returns the buffer. 
    //

    //
    // Read from the connection
    //
	try
	{
		buffer = 0;
		mTcpServer->readFromNew(buffer, client);

		OmnTraceComm << "Read successful from: "
			<< (client->getRemoteIpAddr()).toString() << ":"
			<< client->getRemotePort() << ":"
			<< buffer->getDataLength() << endl;

		//
		// Should never come to this point
		//
		return true;
	}

	catch (const OmnExcept &e)
	{
		e.procException();
		buffer = 0;
		return OmnAlarm << e.getErrid()
			<< "Failed to create buffer. It may run out of memory." << enderr;
	}

	buffer = 0;
	return OmnShouldNeverComeToThisPoint;
}


bool
OmnTcpCommSvr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTraceThread << "Enter OmnTcpCommSvrReadThreadFunc" << endl;

    OmnConnBuffPtr buff;
    OmnRslt rslt;
	OmnTcpClientPtr client;
    while (state == OmnThrdStatus::eActive)
    {
        rslt = readFrom(buff, client);
        if (!rslt)
        {

            //
            // Failed to read
            // Currently we wait for eReadFailIntervalTimerSec seconds
            // and then try it again.
            //
            OmnSleep(OmnCommTypes::eReadFailIntervalTimerSec);
            continue;
        }

        //
        // It read successfully.
        //
        if (!mListener)
        {
            //
            // This should not happen
            //
            OmnAlarm << OmnErrId::eListenerNull
				<< "Read something but requester is null" << enderr;
            continue;
        }
        else
        {
            mListener->msgRecved(buff, client);
        }
    }

    OmnTrace << "Leave OmnOmnPhoneMsgProc::threadFunc" << endl;
    return true;
}


bool
OmnTcpCommSvr::signal(const int threadLogicId)
{
    //
    // Don't need to do anything
    //
    return true;
}


OmnRslt
OmnTcpCommSvr::startReading(const OmnTcpListenerPtr &callback)
{
    //
    // There are two methods to read messages from a comm. One is to call
    // the member function: readFrom(...) directly and the other is to
    // use this function.
    //
    // This function checks whether a reading thread has been created. If not,
    // it create one. If the thread is already created, it starts the thread.
    //

    if (!mListener.isNull() && mListener.getPtr() != callback.getPtr())
    {
        //
        // This may be a problem.
        //
        OmnAlarm << OmnErrId::eRegisterListenerError
            << "Found start reading override! The old requester: "
            << mListener->getTcpListenerName()
            << " and the new Requester: " 
			<< callback->getTcpListenerName() << enderr;
    }

    mListener = callback;

    OmnTraceComm << callback->getTcpListenerName()
        << " start reading on: "
        << mTcpServer->getLocalIpAddr().toString()
        << ":" << mTcpServer->getLocalPort() << endl;

    if (mThread.isNull())
    {
        //
        // The thread has not been created yet.
        //
		OmnThreadedObjPtr thisPtr(this, false);
        mThread = OmnNew OmnThread(thisPtr, mName, 0, false, false, __FILE__, __LINE__);
    }

    if (mThread->getStatus() != OmnThrdStatus::eActive)
    {
        mThread->start();
    }

    return true;
}


OmnRslt
OmnTcpCommSvr::stopReading(const OmnTcpListenerPtr &requester)
{
    if (mThread.isNull())
    {
        //
        // There is no reading thread.
        //
        return true;
    }

    if (mListener != requester)
    {
        return OmnAlarm << OmnErrId::eStopReadingError
			<< "To stop a thread that is not owned by the requester"
            << enderr;
    }

    mListener = 0;
    return mThread->stop();
}


OmnRslt
OmnTcpCommSvr::forceStop()
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

    return mThread->stop();

    mListener = 0;
}


OmnRslt
OmnTcpCommSvr::reconnect(OmnString &err)
{
	OmnNotImplementedYet;
	return false;
}


void
OmnTcpCommSvr::heartbeat()
{
	//
	// Currently not managed by ThreadMgr. Do nothing
	//
}


bool
OmnTcpCommSvr::checkThread(OmnString &err, const int thrdLogicId) const
{
	//
	// Currently not managed by ThreadMgr. Always return true
	//
    return true;
}


bool
OmnTcpCommSvr::isCriticalThread() const
{
    return false;
}


OmnIpAddr
OmnTcpCommSvr::getLocalIpAddr() const
{
	return mLocalAddr;
}


int
OmnTcpCommSvr::getLocalPort() const
{
	return mLocalPort;
}

