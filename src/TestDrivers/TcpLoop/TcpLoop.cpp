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
// A TCP connection contains two objects: TCP Client and TCP Server. 
// This is normally used for testing.   
//
// Modification History:
// 08/04/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TcpLoop/TcpLoop.h"

#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCommClt.h"
#include "UtilComm/ConnBuff.h"


OmnTcpLoop::OmnTcpLoop()
{
}


OmnTcpLoop::OmnTcpLoop(const OmnIpAddr &remoteAddr, 
					   const u32 remotePort,
					   const OmnIpAddr &clientAddr, 
					   const u32 clientPort, 
					   const OmnIpAddr &serverAddr, 
					   const u32 serverPort, 
					   const BounceMethod method, 
					   const u32 sendBlockSize, 
					   const int sendSize,
					   const u32 sendWaitSec, 
					   const u32 sendWaitUsec)
:
mServerAddr(serverAddr),
mClientAddr(clientAddr),
mRemoteAddr(remoteAddr),
mServerPort(serverPort),
mClientPort(clientPort),
mRemotePort(remotePort),
mBounceMethod(method),
mSendBlockSize(sendBlockSize),
mSendSize(sendSize),
mSendWaitSec(sendWaitSec),
mSendWaitUsec(sendWaitUsec),
mTotalReceived(0),
mLastTotal(0)
{
	if (mSendBlockSize > eMaxSendBlockSize)
	{
		OmnAlarm << "Send Block Size too big: " << mSendBlockSize
			<< ". Maximum allowed: " << eMaxSendBlockSize 
			<< ". Reset to the maximum. " << enderr;
		mSendBlockSize = eMaxSendBlockSize;
	}

	mSendData.set('c', mSendBlockSize);
}


OmnTcpLoop::~OmnTcpLoop()
{
}


bool
OmnTcpLoop::connect(OmnString &errmsg)
{
	return createServer(errmsg) && createClient(errmsg);
}


bool
OmnTcpLoop::createServer(OmnString &errmsg)
{
	if (mServer) return true;

	mServer = OmnNew OmnTcpServer(mServerAddr, mServerPort, 1, "Server");
	if (!mServer)
	{
		errmsg << "Failed to allocate object. Most likely run out of memory";
		return false;
	}

	if (!mServer->connect(errmsg))
	{
		mServer = 0;
		return false;
	}

	return true;
}


bool
OmnTcpLoop::createClient(OmnString &errmsg)
{
	mClient = OmnNew OmnTcpCommClt(mClientAddr, mClientPort, 
				mRemoteAddr, mRemotePort, "Client");
	if (!mClient)
	{
		errmsg << "Failed to allocate client. Most likely run out of memory";
		return false;
	}

	if (!mClient->connect(errmsg))
	{
		mClient = 0;
		return false;
	}

	return true;
}


bool
OmnTcpLoop::start()
{
	if (!mSendingThread)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mSendingThread = OmnNew OmnThread(thisPtr, "TcpLoop", 0, false, false);
		if (!mSendingThread)
		{
			OmnAlarm << "Failed to create the sending thread"
				<< ". Most likely run out of memory" << enderr;
			return false;
		}

		if (!mSendingThread->start())
		{
			OmnAlarm << "Failed to start the sending thread" << enderr;
			return false;
		}
	}

	if (!mClient)
	{
		OmnAlarm << "Client is null" << enderr;
		return false;
	}

	OmnCommListenerPtr thisPtr(this, false);
	if (!mClient->startReading(thisPtr))
	{
		OmnAlarm << "Failed to start reading" << enderr;
		return false;
	}

	OmnTcpListenerPtr listener(this, false);
	mServer->setListener(listener);
	mServer->startReading();
	return true;
}


void        
OmnTcpLoop::msgRecved(const OmnConnBuffPtr &buff,
			   		  const OmnTcpClientPtr &conn)
{
	// 
	// This function is called when the server receives something. 
	//
	OmnTrace << "Received data. Remote: " << buff->getRemoteAddr().toString()
		<< ":" << buff->getRemotePort()
		<< ". Local: " << buff->getLocalAddr().toString()
		<< ":" << buff->getLocalPort() << endl;

	switch (mBounceMethod)
	{
	case eSimpleBounce:
		 if (!conn->writeTo(buff->getData(), buff->getDataLength()))
		 {
			 OmnAlarm << "Failed to send!" << enderr;
			 return;
		 }
		 return;

	case eDrop:
		 return;

	default:
		 OmnAlarm << "Unrecognized bouncing method: " << mBounceMethod << enderr;
		 return;
	}

	return;
}


bool        
OmnTcpLoop::msgRead(const OmnConnBuffPtr &buff)
{
	// 
	// This function is called when the client receives something.
	//
	OmnTrace << "Client received data. Remote: " 
		<< ":" << buff->getRemotePort() 
		<< ". Local: " << buff->getLocalAddr().toString()
		<< ":" << buff->getLocalPort() << endl;
	mTotalReceived += buff->getDataLength();
	mLastTotal += buff->getDataLength();
	return true;
}


bool    
OmnTcpLoop::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnString errmsg;

	while (state == OmnThrdStatus::eActive)
	{
		if (!mServer)
		{
			errmsg = "";
			if (!createServer(errmsg))
			{
				OmnAlarm << "Failed to create Server: " << errmsg << enderr;
				OmnSleep(eFailedCreateServerSleep);
			}

			continue;
		}

		if (!mClient)
		{
			errmsg = "";
			if (!createClient(errmsg))
			{
				OmnAlarm << "Failed to create client: " << errmsg << enderr;
			}

			OmnSleep(eFailedCreateClientSleep);
			continue;
		}

		if (!mClient->sendTo(mSendData, mSendData.length()))
		{
			OmnAlarm << "Failed to send: " << mClient->toString() << enderr;
			mClient = 0;
			OmnSleep(eFailedToSendSleep);
			continue;
		}

		OmnWait::getSelf()->wait(mSendWaitSec, mSendWaitUsec);
	}

	return true;
}


bool    
OmnTcpLoop::signal(const int threadLogicId)
{
	return true;
}


void
OmnTcpLoop::heartbeat(const int threadLogicId)
{
}


bool    
OmnTcpLoop::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


u64			
OmnTcpLoop::getLastTotal() const 
{
	cout << "Conn: " << mLastTotal << endl;
	return mLastTotal;
}

