////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpBouncerEntry.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "BouncerMgr/UdpBouncerEntry.h"

#include "BouncerMgr/BouncerConn.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"


AosUdpBouncerEntry::AosUdpBouncerEntry(const OmnIpAddr& senderIp, 
									   const int& senderPort)
:
AosBouncerEntry::AosBouncerEntry(senderIp,senderPort,"udp")
{
}
	
AosUdpBouncerEntry::~AosUdpBouncerEntry()
{
}	

bool	
AosUdpBouncerEntry::start()
{

	
	mUdpComm = OmnNew OmnUdpComm(mRecverIp,mRecverPort,"TcpBouncer");

	OmnCommListenerPtr thisPtr(this, false);
	mUdpComm->startReading(thisPtr);
	return true;
}

bool	
AosUdpBouncerEntry::stop()
{
    OmnCommListenerPtr thisPtr(this, false);
	mUdpComm->stopReading(thisPtr);	
	mUdpComm->closeConn();	
	mUdpComm = 0;
	return true;
}

bool        
AosUdpBouncerEntry::msgRead(const OmnConnBuffPtr &buff)
{
	OmnIpAddr senderIp = buff->getRemoteAddr();
	int senderPort = buff->getRemotePort();
	
	if(buff->getLocalAddr() != mRecverIp ||
	   buff->getLocalPort() != mRecverPort)	
	{
		return false;
	}
	
	
	// do statistics to conn
	AosBouncerConnPtr bouncerConn;
	bool connExist = false;

	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		bouncerConn = mConnList.crtValue();
		if(bouncerConn->connExist(senderIp,senderPort))
		{
			connExist = true;
			bouncerConn->getPacket(buff,"udp");
			break;
		}
		mConnList.next();
	}

	mLock->unlock();
	
	return true;
}

OmnString   
AosUdpBouncerEntry::getCommListenerName() const
{
	return "UdpBouncerEntry";
}
