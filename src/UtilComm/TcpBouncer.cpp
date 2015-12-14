////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncer.cpp
// Description:
//	This class is a TCP server. It uses a TcpServerGrp to handle
//  all the connections. When running, it listens on the given 
//  IP address and port. When it receives a new connection request,
//  it checks whether the current number of connections has reached
//  the maximum. If not, it accepts the connection. 
//
//	Connections are grouped using OmnTcpCltGrp. Each group may 
//	contain up to 50 connections. Each group runs its own thread
//  to read messages. 
//
//  When a connection receives something, it will call the function
//	"msgRecved(...)". This function checks whether a callback has
//  been registered. If yes, it calls the callback. Otherwise, it
//  simply bounces the contents back to the sender.
//	
//  To register a callback, call "registerCallback(...)". 
//
//	Usage:
//		1. Create an instance of AosTcpBouncer
//		2. Call "connect(...)"
//		3. If wanted, register a callback by calling "registerCallback(...)"
//		4. Call "startReading(...)"
//
//	Example:
//	int test()
//	{
//    	AosTcpBouncer theBouncer("TcpBouncer", 
//			OmnIpAddr("127.0.0.1"), 5000, 5, 500);
//    	OmnString errmsg;
//    	if (!theBouncer.connect(errmsg))
//    	{
//        	cout << "********* Failed to connect: " << errmsg << endl;
//        	return -1;
//    	}
//
//    	theBouncer.startReading();
//		return 0;
//	}   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpBouncer.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/BandWidthMgr.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpBouncerListener.h"


AosTcpBouncer::AosTcpBouncer(const OmnString &name,
            const OmnIpAddr &localAddr,
            const int localPort,
            const int numLocalPorts,
            const int maxConns,
            const BounceType bm, 
			const u32 expectedBytes)
:
mName(name),
mLocalAddr(localAddr),
mLocalPort(localPort),
mNumLocalPorts(numLocalPorts),
mLock(OmnNew OmnMutex()),
mBounceType(bm),
mExpectedBytes(expectedBytes),
mRunAttacker(false)
{
	mServer = OmnNew OmnTcpServerGrp(localAddr, localPort,
                 numLocalPorts, name, maxConns, 
				 eAosTLT_NoLengthIndicator);
	OmnTcpCltGrpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
}


AosTcpBouncer::~AosTcpBouncer()
{
	stopReading();
}


bool
AosTcpBouncer::connect(OmnString &errmsg)
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
AosTcpBouncer::msgRecved(const OmnTcpCltGrpPtr &group, 
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

	AosTcpBouncerClientPtr client = mClients[sock];
	if (!client)
	{
		mLock->unlock();
		aos_alarm("Received something but no client found: %d",
			conn->getSock());
		return;
	}

	mLock->unlock();
	client->msgReceived(buff);
}


bool
AosTcpBouncer::startReading()
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
AosTcpBouncer::stopReading()
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
AosTcpBouncer::newConn(const OmnTcpClientPtr& conn)
{
	// 
	// A new connection has been created. It creates a bounce
	// client and set the connection to it.
	//
	mLock->lock();
	int sock = conn->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		mLock->unlock();
		aos_alarm("Invalid sock: %d", sock);
		return;
	}

	if (mClients[sock])
	{
		// 
		// There are still clients for the sock. This is an error.
		//
		aos_alarm("There is still a client for the sock: %d", sock);
		mClients[sock] = 0;
		mServer->removeConn(conn);
		mLock->unlock();
		return;
	}

	AosTcpBouncerPtr selfPtr(this, false);
	AosTcpBouncerClientPtr client = OmnNew AosTcpBouncerClient(
		selfPtr, conn, mBounceType, mExpectedBytes);
	client->registerListener(mListener);
	mClients[sock] = client;
	mLock->unlock();

	if (!client)
	{
		aos_alarm("Memory error");
		return;
	}
	
	// 
	// Inform the listener
	//
	if (mListener)
	{
		mListener->clientCreated(client);
	}

	return;
}


bool		
AosTcpBouncer::registerCallback(const AosTcpBouncerListenerPtr &callback)
{
	if (mListener)
	{
		OmnAlarm << "The callback is not null" << enderr;
	}

	mListener = callback;
	return true;
}


int
AosTcpBouncer::removeClient(const AosTcpBouncerClientPtr &client)
{
	int sock = client->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		return aos_alarm("Invalid sock: %d", sock);
	}

	mLock->lock();
	if (!mClients[sock])
	{
		mLock->unlock();
		return aos_alarm("Client is not set in its position: %d", sock);
	}

	mClients[sock] = 0;
	mLock->unlock();

	if (mListener)
	{
		mListener->clientClosed(client);
	}

	return 0;
}


bool        
AosTcpBouncer::connClosed(const OmnTcpCltGrpPtr &, 
                            const OmnTcpClientPtr &conn)
{
	int sock = conn->getSock();
	if (sock < 0 || sock > eMaxSock)
	{
		return aos_alarm("Invalid sock: %d", sock);
	}

	mLock->lock();
	AosTcpBouncerClientPtr client = mClients[sock];
	if (!client)
	{
		aos_alarm("To remove a client but client is null: %d", 
			sock);
		return false;
	}

	mClients[sock] = 0;
	mLock->unlock();

	if (mListener)
	{
		mListener->clientClosed(client);
	}

	return true;
}


bool		
AosTcpBouncer::addAttacker(const OmnIpAddr &localIP,const int &port)
{
	for(int i = 0;i<eMaxAttacker;i++)
	{
		if(mPinholeAttacker[i].isNull())
		{
			mPinholeAttacker[i] = OmnNew OmnTcpClient(
				OmnIpAddr::eInvalidIpAddr,
				0,
				1,
				localIP,
				port,
				1,
				"PinholeAttacker",
				eAosTLT_LengthIndicator);
			return true;
		}
		
	}
	return false;
}

bool		
AosTcpBouncer::removeAttacker(const OmnIpAddr &localIP,const int &port)
{
	bool found = false;
	for(int i = 0;i<eMaxAttacker;i++)
	{
		if(found)
		{
			if(i < eMaxAttacker - 1)
			{
				mPinholeAttacker[i] = mPinholeAttacker[i+1];
			}
			else
			{
				mPinholeAttacker[i] = 0;
			}
			continue;
		}
		if(localIP == mPinholeAttacker[i]->getLocalIpAddr() &&
		   port == mPinholeAttacker[i]->getLocalPort())
		{
			found = true;
			mPinholeAttacker[i]->closeConn();
			mPinholeAttacker[i] = mPinholeAttacker[i+1];			
		}
	}
	return true;
}

void		
AosTcpBouncer::startAttackers()
{
/*	for(int i = 0;i<eMaxAttacker;i++)
	{
		if(mPinholeAttacker[i].isNull()
		{
			break;
		}
		mPinholeAttacker[i]->stopReading();
	}
*/
	mRunAttacker = true;
}

void		
AosTcpBouncer::stopAttackers()
{
	mRunAttacker = false;
}

bool		
AosTcpBouncer::needAttackPinhole()
{
	return mRunAttacker;
}


bool		
AosTcpBouncer::attackPinhole(const OmnIpAddr &remoteIp,const int &remotePort)
{
	for(int i = 0;i<eMaxAttacker;i++)
	{
		if(mPinholeAttacker[i].isNull())
		{
			break;
		}
		OmnTcpClientPtr curAttacker = mPinholeAttacker[i];
		if(remoteIp == curAttacker->getRemoteIpAddr() &&
		   remotePort == curAttacker->getRemotePort())
		{
			if(curAttacker->isConnGood())
			{
				sendPinholeAttackMsg(curAttacker);
			}
		}
		if(curAttacker->isConnGood())
		{
			//remote ip and port is changed, so break the connection first
			curAttacker->closeConn();						
		}
		curAttacker->setRemoteIpAddr(remoteIp);
		curAttacker->setRemotePort(remotePort);
		OmnString errMsg;
		if(curAttacker->connect(errMsg))
		{
			sendPinholeAttackMsg(curAttacker);
		}
	}

	return true;
}

bool
AosTcpBouncer::sendPinholeAttackMsg(const OmnTcpClientPtr &attacker)
{
	if(attacker.isNull())
	{
		return true;
	}
	OmnString msg;
	msg << attacker->getLocalIpAddr().toString()
		<< " " << attacker->getLocalPort()
		<< " " << attacker->getRemoteIpAddr().toString()
		<< " " << attacker->getRemotePort();
	//attacker->writeTo(msg.getBuffer(),msg.length());
	return true;
}

void
AosTcpBouncer::setBWMgr(const AosBandWidthMgrPtr &bwMgr)
{
	mBandWidthMgr = bwMgr;
}

AosBandWidthMgrPtr
AosTcpBouncer::getBWMgr()const 
{
	return mBandWidthMgr;
}
