////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpStreamSimuEntry.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "StreamSimu/UdpStreamSimuEntry.h"

#include "StreamSimu/StreamSimuConn.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/Udp.h"
#include "UtilComm/ConnBuff.h"


AosUdpStreamSimuEntry::AosUdpStreamSimuEntry(const OmnIpAddr& senderIp, 
									   const int& senderPort)
:
AosStreamSimuEntry::AosStreamSimuEntry(senderIp,senderPort,"udp", 0)
{
}
	
AosUdpStreamSimuEntry::~AosUdpStreamSimuEntry()
{
}	

bool	
AosUdpStreamSimuEntry::start()
{
	mUdp = OmnNew OmnUdp("UdpStreamSimuEntry",
						 mSenderIp,
						 mSenderPort);
	OmnString errMsg;
	mUdp->connect(errMsg);

/*	mUdpComm = OmnNew OmnUdpComm(mRecverIp,mRecverPort,"TcpStreamSimu");
	OmnString errMsg;
	mUdpComm->connect(errMsg);

    OmnCommListenerPtr thisPtr(this, false);
	mUdpComm->startReading(thisPtr);
*/	return true;
}

bool	
AosUdpStreamSimuEntry::stop()
{
	mUdp->closeConn();

	return true;
}

bool    
AosUdpStreamSimuEntry::sendPacket(const OmnIpAddr &recverIp, 
								  const int recverPort,      
								  const int size)
{
	if(size < 0 || size > eMaxPacketSize)
	{
		return false;
	}
	mLock->lock();
	mUdp->writeTo(mContent,size,recverIp,recverPort);
	mLock->unlock();
	return true;
}

