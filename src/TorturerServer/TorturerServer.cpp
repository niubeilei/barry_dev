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
// 01/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerServer/TorturerServer.h"

#include "alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "TorturerServer/TSTrans.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/ReturnCode.h"


AosTorturerServer::AosTorturerServer(const OmnString &name,
            const OmnIpAddr &localAddr,
            const int localPort,
            const int numLocalPorts)
:
mName(name),
mLocalAddr(localAddr),
mLocalPort(localPort),
mNumLocalPorts(numLocalPorts),
mLock(OmnNew OmnMutex())
{
	mServer = OmnNew OmnTcpServerGrp(localAddr, localPort,
                 numLocalPorts, name, 50, 
				 OmnTcp::eNoLengthIndicator);
	OmnTcpCltGrpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
}


AosTorturerServer::~AosTorturerServer()
{
	stopReading();
}


bool
AosTorturerServer::connect(OmnString &errmsg)
{
	if (!mServer)
	{
		errmsg = "Server not created before connecting";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return mServer->connect(errmsg);
}


void
AosTorturerServer::msgRecved(const OmnTcpCltGrpPtr &group, 
						 const OmnConnBuffPtr &buff,
						 const OmnTcpClientPtr &conn)
{
	// 
	// Retrieve the client
	//
	mLock->lock();
	int sock = conn->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		mLock->unlock();
		aos_alarm("Invalid sock: %d", sock);
		return;
	}

	AosTSTransPtr client = mTrans[sock];
	if (!client)
	{
		mLock->unlock();
		aos_alarm("Received something but no transaction found: %d",
			conn->getSock());
		return;
	}

	mLock->unlock();
	client->msgReceived(buff, conn);
}


bool
AosTorturerServer::startReading()
{
	if (!mServer)
	{
		OmnAlarm << "Server not created before startReading()" << enderr;
		return false;
	}

	mServer->startReading();
	return true;
}


bool
AosTorturerServer::stopReading()
{
	if (!mServer)
	{
		OmnAlarm << "Server not created before stopReading()" << enderr;
		return false;
	}

	mServer->stopReading();
	return true;
}


void
AosTorturerServer::newConn(const OmnTcpClientPtr& conn)
{
	// 
	// A new connection has been created. It creates a transaction
	// and set the connection to it.
	//
	mLock->lock();
	int sock = conn->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		mLock->unlock();
		aos_alarm("Invalid sock: %d", sock);
		return;
	}

	if (mTrans[sock])
	{
		// 
		// There are still clients for the sock. This is an error.
		//
		aos_alarm("There is still a client for the sock: %d", sock);
		mTrans[sock] = 0;
		mServer->removeConn(conn);
		mLock->unlock();
		return;
	}

	AosTorturerServerPtr selfPtr(this, false);
	AosTSTransPtr client = OmnNew AosTSTrans(selfPtr, conn);
	mTrans[sock] = client;
	mLock->unlock();

	if (!client)
	{
		aos_alarm("Memory error");
		return;
	}
	
	return;
}


int
AosTorturerServer::removeClient(const AosTSTransPtr &client)
{
	int sock = client->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		return aos_alarm("Invalid sock: %d", sock);
	}

	mLock->lock();
	if (!mTrans[sock])
	{
		mLock->unlock();
		return aos_alarm("Client is not set in its position: %d", sock);
	}

	mTrans[sock] = 0;
	mLock->unlock();

	return 0;
}


bool        
AosTorturerServer::connClosed(const OmnTcpCltGrpPtr &, 
                            const OmnTcpClientPtr &conn)
{
	int sock = conn->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		return aos_alarm("Invalid sock: %d", sock);
	}

	mLock->lock();
	AosTSTransPtr client = mTrans[sock];
	if (!client)
	{
		aos_alarm("To remove a client but transaction is null: %d", 
			sock);
		return false;
	}

	mTrans[sock] = 0;
	mLock->unlock();

	return true;
}

