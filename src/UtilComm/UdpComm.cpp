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
//	This class maintains only one local port. All messages received
//  from this port must belong to the same message category, which 
//  is identified by mMsgCat.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/UdpComm.h"

#include "Alarm/Alarm.h"
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
#include "UtilComm/CommTypes.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/CommGroup.h"
#include "XmlParser/XmlItem.h"
#include "XmlUtil/XmlItemName.h"


static int sgReadingTimer = 30;


OmnUdpComm::OmnUdpComm(
		const OmnCommProt::E protocol,
		const bool proxySenderAddr,
		const OmnXmlItemPtr &def,
		const int targetId,
		const OmnNetEtyType::E targetType,
		const OmnCommGroupPtr &comm)
:
OmnComm(protocol, proxySenderAddr, comm, targetId, targetType)
{
	OmnTraceComm << "Created a UdpComm. Target ID: " << targetId
		<< " target type: " << mTargetType << endl;

	OmnRslt rslt = config(def);
	if (!rslt)
	{
		return;
	}

	mUdpConn = OmnNew OmnUdp(mName, mLocalIpAddr, mLocalPort);

	OmnString err;
	rslt = mUdpConn->connect(err);
	if (!rslt)
	{
		OmnWarn << OmnErrId::eWarnCommError << err << enderr;
		return;
	}

	mLocalPort = mUdpConn->getLocalPort();
}


OmnUdpComm::OmnUdpComm(
		const OmnString &name,
		const OmnCommProt::E protocol,
		const bool proxySenderAddr,
		const OmnIpAddr &localAddr,
		const int localPort,
		const OmnCommGroupPtr &comm)
:
OmnComm(protocol, proxySenderAddr, comm, -1, OmnNetEtyType::eNoEntityType),
mLocalIpAddr(localAddr),
mLocalPort(localPort)
{
	mName = name;
	mUdpConn = OmnNew OmnUdp(mName, mLocalIpAddr, mLocalPort);

	OmnString err;
	OmnRslt rslt = mUdpConn->connect(err);
	if (!rslt)
	{
		OmnWarn << err << enderr;
		return;
	}

	mLocalPort = mUdpConn->getLocalPort();
}


OmnUdpComm::OmnUdpComm(
		const OmnIpAddr &localAddr, 
		const int port,
		const OmnString &name)
:
OmnComm(OmnCommProt::eUDP, name),
mLocalIpAddr(localAddr),
mLocalPort(port)
{
	// 
	// Make sure the IP address and port are correct.
	//
	if (!localAddr.isValid())
	{
		OmnAlarm << "Incorrect local address: " << localAddr.toString() 
			<< " for " << name << enderr;
	}

	if (mLocalPort < 0)
	{
		OmnAlarm << "Incorrect local port: " << mLocalPort 
			<< " for: " << name << enderr;
	}

	OmnTraceComm << "Created a UdpComm: " 
		<< localAddr.toString() << ":" 
		<< port << ":" 
		<< name << endl;

	mUdpConn = OmnNew OmnUdp(mName, mLocalIpAddr, mLocalPort);

	OmnString err;
	OmnRslt rslt = mUdpConn->connect(err);
	if (!rslt)
	{
		OmnWarn << err << enderr;
		return;
	}

	mLocalPort = mUdpConn->getLocalPort();
}


OmnUdpComm::~OmnUdpComm()
{
}


OmnRslt
OmnUdpComm::config(const OmnXmlItemPtr &def)
{
	//
	// It assumes the following:
	//
	//	<Comm>
	//		<Name>			; Optional
	//		<CommType>		; should be UdpComm
	//		<NIID>
	//		<LocalPort>
	//		<RemoteIpAddr>	; optional
	//		<RemotePort>	; optional
	//	</Comm>
	//
/*
	// Retrieve Name
	mName = def->getStr(OmnXmlItemName::eName, "NoName");
	
	//
	// Retrieve NIID. 
	//
	int niid = def->getInt(OmnXmlItemName::eNIID, -1);
	mNetInterface = OmnNetworkSelf->getNetInterface(niid);
	if (mNetInterface.isNull())
	{
		//
		// Invalid net interface id.
		//
		return OmnAlarm << "Interface invalid: "
			<< def->toString() << enderr;
	}

	mLocalIpAddr = mNetInterface->getIpAddr(); 

	//
	// Retrieve local port. If local port not present, we will ask the
	// system to allocate a port for it.
	//
	mLocalPort = def->getInt(OmnXmlItemName::eLocalPort, 0);

	// Retrieve Remote IpAddr, Optional
	mRemoteIpAddr = def->getIpAddr(OmnXmlItemName::eRemoteIpAddr, 
						OmnIpAddr::eInvalidIpAddr);	

	//
	// Retrieve remote port
	//
	mRemotePort = def->getInt(OmnXmlItemName::eRemotePort, -1);

	OmnTrace << "Create UdpComm. Local: " 
		<< mLocalIpAddr.toString() << ":" << mLocalPort
		<< ", Remote: " << mRemoteIpAddr.toString()
		<< ":" << mRemotePort << endl;
	//
	// Configuration successful
	//
	return true;
	*/

	return false;
}


OmnRslt				
OmnUdpComm::sendBuff(
		const OmnIpAddr &recvAddr, 
		const int recvPort,
		const OmnString &buff)
{
	// If the message contains non-zero remote (receiver) port, 
	// this function uses the IP address and port from the message.
	// Otherwise, it uses mRemoteIpAddr and mRemotePort to send.
	OmnTraceComm << "Send a buff to: " 
		<< recvAddr.toString() << ":" << recvPort
		<< ". Content: \n" << buff.getBuffer() << endl;
	return mUdpConn->writeTo(buff.getBuffer(), buff.length(), 
		recvAddr, recvPort);
}


OmnRslt
OmnUdpComm::sendTo(
		const char *data, 
		const int length,
		const OmnIpAddr &remoteIpAddr,
		const int remotePort)
{
	if (remotePort <= 0)
	{
		// It should use this group's receiver IP address and port
		return mUdpConn->writeTo(data, length, mRemoteIpAddr, mRemotePort);
	}
	else
	{
		// Otherwise, use the specified IP address and port
		return mUdpConn->writeTo(data, length, remoteIpAddr, remotePort);
	}
}


OmnString
OmnUdpComm::toString() const
{
	if (mUdpConn.isNull())
	{
		return OmnString("UDP Single Port Communicator: ")
			   << mName << ". Connection is null!";
	}
	else
	{
		return OmnString("UDP Single PortCommunicator: ")
			   << mName << ". UDP Connection Info: " 
			<< mUdpConn->toString();
	}
}


bool		
OmnUdpComm::isConnGood() const
{
	if (!mUdpConn.isNull() && mUdpConn->isConnGood())
	{
		return true;
	}

	return false;
}


OmnRslt		
OmnUdpComm::closeConn()
{
	return mUdpConn->closeConn();
}


OmnRslt		
OmnUdpComm::connect(OmnString &err)
{
	OmnRslt rslt = mUdpConn->connect(err);
	mLocalPort = mUdpConn->getLocalPort();
	return rslt;
}


OmnRslt
OmnUdpComm::reconnect(OmnString &err)
{
	return mUdpConn->reconnect(err);
}


OmnRslt 
OmnUdpComm::readFrom(
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

	if (mProxySenderAddr)
	{
		buffer->restoreSenderAddr();
	}
	//else
	//{
	//	buffer->copySenderAddr();
	//}

	OmnTraceComm << "Read successful from: " 
		<< mLocalIpAddr.toString() << ":" 
		<< mLocalPort 
		<< ", " << (buffer->getRemoteAddr()).toString()
		<< ":" << buffer->getRemotePort() << ":" << buffer->getDataLength() << endl;

	// Should never come to this point
	return true;
}


bool 
OmnUdpComm::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTraceThread << "Enter OmnUdpCommReadThreadFunc" << endl;

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
		else
		{
			mRequester->msgRead(buff);
		}
	}

	OmnTrace << "Leave OmnOmnPhoneMsgProc::threadFunc" << endl;
	return true;
}


bool 
OmnUdpComm::signal(const int threadLogicId)
{
	// Don't need to do anything
	return true;
}


OmnRslt
OmnUdpComm::startReading(const OmnCommListenerPtr &callback)
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

	OmnTraceComm << callback->getCommListenerName()
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
OmnUdpComm::stopReading(const OmnCommListenerPtr &requester)
{
	if (mReadingThread.isNull())
	{
		// There is no reading thread.
		return true;
	}

	if (mRequester != requester)
	{
		return OmnAlarm << "To stop a thread that is not owned by the requester"
			<< enderr;
	}

	mRequester = 0;
	return mReadingThread->stop();
}


OmnRslt
OmnUdpComm::forceStop()
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
OmnUdpComm::checkThread(OmnString &, const int tid) const
{
	// Since it is not managed by ThreadMgr, we always return true.
    return true;
}


int
OmnUdpComm::getSock() const
{
	if (mUdpConn.isNull())
	{
		return -1;
	}

	return mUdpConn->getSock();
}

