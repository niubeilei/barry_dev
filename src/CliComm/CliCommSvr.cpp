////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommSvr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliComm/CliCommSvr.h"

#include "CliComm/CliSession.h"
#include "CliComm/CliRequest.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


AosCliCommSvr::AosCliCommSvr(const OmnString &name, 
					   const OmnIpAddr &localAddr, 
					   const int localPort, 
					   const int numLocalPorts)
:
mName(name),
mLock(OmnNew OmnMutex()),
mLocalAddr(localAddr),
mLocalPort(localPort),
mNumLocalPorts(numLocalPorts)
{
	mServer = OmnNew OmnTcpServerGrp(localAddr, localPort,
                 numLocalPorts, name, OmnTcp::eNoLengthIndicator);
	OmnTcpCltGrpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
}


bool
AosCliCommSvr::connect(OmnString &errmsg)
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
AosCliCommSvr::msgRecved(const OmnTcpCltGrpPtr &group, 
						 const OmnConnBuffPtr &buff,
						 const OmnTcpClientPtr &client)
{
	// 
	// Bounce the contents back.
	// 
	OmnTrace << "Received: " << buff->getDataLength() << endl;

	// 
	// Find the session first.
	//
	AosCliSessionPtr session = getSession(client);
	if (!session)
	{
		OmnAlarm << "Received data but no session found!" << enderr;
		return;	
	}

	session->msgRecved(client, buff);
	return;
}


void
AosCliCommSvr::newConn(const OmnTcpClientPtr &client)
{
	AosCliSessionPtr session = OmnNew AosCliSession(client);
	client->setLengthType(eAosTLT_NoLengthIndicator);
	
OmnTrace << "New conn: " << client->getSock() << endl;

	mLock->lock();
	mSessions.append(session);
	mLock->unlock();
}
	

bool
AosCliCommSvr::startReading()
{
	if (!mServer)
	{
		OmnAlarm << "Server not created before startReading()" << enderr;
		return false;
	}

	mServer->startReading();
	return true;
}


AosCliSessionPtr
AosCliCommSvr::getSession(const OmnTcpClientPtr &client)
{
	mLock->lock();
	mSessions.reset();
	while (mSessions.hasMore())
	{
		AosCliSessionPtr session = mSessions.next();
		if (session->getSock() == client->getSock())
		{
			mLock->unlock();
			return session;
		}
	}

	mLock->unlock();
	return 0;
}


bool
AosCliCommSvr::connAccepted(const OmnTcpClientPtr &client)
{
	AosCliSessionPtr session = getSession(client);
	if (session)
	{
		OmnAlarm << "Session already in the list: " 
			<< client->getSock() << enderr;
		return false;
	}

	session = OmnNew AosCliSession(client);
	if (!session)
	{
		OmnAlarm << "Failed to allocate a new instance of CliSession"
			<< enderr;
		return false;
	}

	mLock->lock();
	mSessions.append(session);
	mLock->unlock();
	return true;
}

