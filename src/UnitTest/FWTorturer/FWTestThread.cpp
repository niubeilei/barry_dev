////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TrafficGenThread.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TrafficGenThread.h"


#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpTrafficGenClient.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/ReturnCode.h"


AosTcpTrafficGenThread::AosTcpTrafficGenThread(
			const AosTcpTrafficGenPtr &trafficGen,
			const int grpIndex, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts,
			const int groupSize, 
			const AosTcpTrafficGenListenerPtr &listener)
:
mGrpIndex(grpIndex),
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mNumPorts(numPorts),
mNumClients(0),
mGroupSize(groupSize),
mLock(OmnNew OmnMutex()),
mGroup(OmnNew OmnTcpCltGrp()),
mTrafficGen(trafficGen),
mListener(listener)
{
}


AosTcpTrafficGenThread::~AosTcpTrafficGenThread()
{
	mGroup->closeReading();
	while(!mGroup->isClosed())
	{
		OmnWait::getSelf()->wait(0,100);
	}
}


bool
AosTcpTrafficGenThread::start()
{
	while(!mGroup->isRight())
	{
		OmnWait::getSelf()->wait(1,0);
		cout << "try to create new tcp client group" << endl;
		mGroup = OmnNew OmnTcpCltGrp(); 
	}
	OmnTcpCltGrpListenerPtr thisPtr(this, false);
	mGroup->setListener(thisPtr);
	mGroup->startReading();

	for (int i=0; i<mNumClients; i++)
	{
		mClients[i]->restart();
	}

	return true;
}


void        
AosTcpTrafficGenThread::msgRecved(const OmnTcpCltGrpPtr &group,
                                  const OmnConnBuffPtr &buff,
                                  const OmnTcpClientPtr &conn)
{
	// 
	// Received a packet from a connection. Check whether the connection
	// has read all it needs to. If yes, close the connection from the 
	// group. After that, it checks whether the client has finished 
	// the repeats. If not, create a new connection and add it to the 	
	// group. 
	//
	AosTcpTrafficGenClientPtr client = getClient(conn);
	if (!client)
	{
		// 	
		// This should not happen. 
		//
		OmnAlarm << "Unrecognized client!" << enderr;
		connClosed(group, conn);
		return;
	}

	// 
	// Found the client. 
	// 
	client->msgReceived(buff);
	if (client->isFinished())
	{
OmnTrace << "Client: " << client->getClientId() << " to close conn" << endl;
		connClosed(group, conn);
		if (client->needToRestart())
		{
			client->restart();
		}
		else
		{
			// 
			// The client has finished. Check whether all 
			// clients have finished.
			//
			mTrafficGen->checkFinish();
		}
	}
}


bool
AosTcpTrafficGenThread::connClosed(
			const OmnTcpCltGrpPtr &group, 
			const OmnTcpClientPtr &conn)
{
	AosTcpTrafficGenClientPtr client = getClient(conn);
	if (!client)
	{
		// 	
		// This should not happen. 
		//
		OmnAlarm << "Unrecognized client!" << enderr;
	}
	else
	{
		if (mListener)
		{
			mListener->connClosed(client, conn);
		}
	}

	group->removeConn(conn);

	return true;
}


AosTcpTrafficGenClientPtr
AosTcpTrafficGenThread::getClient(const OmnTcpClientPtr &conn)
{
	mLock->lock();
	int index = (int)conn->getUserData();
	if (index < 0 || index >= eMaxClient)
	{
		mLock->unlock();
		OmnAlarm << "Index out of bound: " << index << enderr;
		return 0;
	}

	AosTcpTrafficGenClientPtr client = mClients[index];
	mLock->unlock();
	return client;
}


void
AosTcpTrafficGenThread::printStatus() const
{
	for (int i=0; i<mNumClients; i++)
	{
		mClients[i]->printStatus();
	}
}


void        
AosTcpTrafficGenThread::newConn(const OmnTcpClientPtr &conn)
{
	return;
}


bool
AosTcpTrafficGenThread::isAllFinished() const
{
	mLock->lock();
	for (int i=0; i<mNumClients; i++)
	{
		if (!mClients[i]->isFinished() || mClients[i]->needToRestart())
		{
			mLock->unlock();
			return false;
		}
	}

	mLock->unlock();
	return true;
}


int			
AosTcpTrafficGenThread::getClients(OmnVList<AosTcpTrafficGenClientPtr> &clients)
{
	for (int i=0; i<mNumClients; i++)
	{
		clients.append(mClients[i]);
	}

	return mNumClients;
}


void		
AosTcpTrafficGenThread::setListener(const AosTcpTrafficGenListenerPtr &listener)
{
	mListener = listener;
}

OmnTcpCltGrpPtr	
AosTcpTrafficGenThread::getGroup() const
{
	return mGroup;
}


int
AosTcpTrafficGenThread::addClient(const AosTcpTrafficGenClientPtr &client)
{
	mLock->lock();
	if (mNumClients >= mGroupSize && mNumClients >= eMaxClient)
	{
		mLock->unlock();
		return -eAosRc_TooManyClients;
	}

	mClients[mNumClients++] = client;
	mLock->unlock();
	return 0;
}


u32 
AosTcpTrafficGenThread::getBytesSent() const
{
	u32 bytes = 0;

	for (int i=0; i<mNumClients; i++)
	{
		bytes += mClients[i]->getBytesSent();
	}

	return bytes;
}


u32 
AosTcpTrafficGenThread::getBytesRcvd() const
{
	u32 bytes = 0;

	for (int i=0; i<mNumClients; i++)
	{
		bytes += mClients[i]->getBytesRcvd();
	}

	return bytes;
}


u32 
AosTcpTrafficGenThread::getTotalConns() const
{
	u32 conns = 0;

	for (int i=0; i<mNumClients; i++)
	{
		conns += mClients[i]->getTotalConns();
	}

	return conns;
}


u32 
AosTcpTrafficGenThread::getFailedConns() const
{
	u32 conns = 0;

	for (int i=0; i<mNumClients; i++)
	{
		conns += mClients[i]->getFailedConns();
	}

	return conns;
}


bool
AosTcpTrafficGenThread::checkConns()
{
	for (int i=0; i<mNumClients; i++)
	{
		mClients[i]->checkConn();
	}

	return true;
}

bool		
AosTcpTrafficGenThread::exit()
{
	mGroup->closeReading();
	return true;
}


bool		
AosTcpTrafficGenThread::isExited()
{
	return mGroup->isClosed();
}

