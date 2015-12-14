////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamSimuEntry.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "StreamSimu/StreamSimuEntry.h"

#include "StreamSimu/StreamSimuConn.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "Util1/Time.h"
#include "Porting/GetTime.h"


char AosStreamSimuEntry::mContent[eMaxPacketSize];


AosStreamSimuEntry::AosStreamSimuEntry(
			const OmnIpAddr	&sendIp,
			const int		&sendPort,
			const OmnString &protocol, 
			const u32        duration)
:
mLock(OmnNew OmnMutex()),
mSenderIp(sendIp),
mSenderPort(sendPort),
mProtocol(protocol),
mStatus(eConnNotCreatedYet),
mBytesSent(0),
mBytesRcvd(0),
mDuration(duration)
{
}								 

AosStreamSimuEntry::AosStreamSimuEntry()
:
mLock(OmnNew OmnMutex()),
mSenderPort(-1), 
mDuration(1)
{
}
	
AosStreamSimuEntry::~AosStreamSimuEntry()
{
}

void		
AosStreamSimuEntry::initContent()
{
	memset(mContent,'a',eMaxPacketSize);
}

bool	
AosStreamSimuEntry::entryExist(const OmnIpAddr 		&senderIp,
					   		const int		  		senderPort,
					   		const OmnString 		&protocol)
{
	return (mSenderIp == senderIp && 
			mSenderPort == senderPort && 
			mProtocol == protocol);
}

bool	
AosStreamSimuEntry::addConn(const OmnIpAddr 					&recverIp,
							const int		  					recverPort,
						    const int		  					bandwidth,
						    const int		  					packetSize,
							const AosStreamType::E 				streamType,
	                     	const int                          	maxBw,
	                        const int                          	minBw)
{
	bool connExist = false;
	AosStreamSimuConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(recverIp,recverPort))
		{
			connExist = true;
			break;
		}
		mConnList.next();
	}
	
	if(!connExist)
	{
		AosStreamSimuEntryPtr thisPtr(this, false);
		conn = OmnNew AosStreamSimuConn(mSenderIp,
									 mSenderPort,
									 recverIp,
									 recverPort,
									 mProtocol,
									 bandwidth,
									 packetSize,
									 streamType,
									 maxBw,
									 minBw,
									 thisPtr);
		mConnList.append(conn);
		conn->start();
	}	
	mLock->unlock();
	return !connExist;	
}

bool	
AosStreamSimuEntry::removeConn(const OmnIpAddr 		&recverIp,
				   			const int		  		recverPort)
{
	bool connExist = false;
	AosStreamSimuConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(recverIp,recverPort))
		{
			connExist = true;
			mConnList.eraseCrt();			
			break;
		}
		conn->stop();
		mConnList.next();
	}
	mLock->unlock();
	return connExist;	
}				   

bool
AosStreamSimuEntry::getBandwidth(const OmnIpAddr 		&recverIp,
				   				 const int		  		recverPort,
								 int			  		&bw,
								 int			  		&packetNum)
{
	AosStreamSimuConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(recverIp,recverPort))
		{
			mLock->unlock();
			return conn->getBandwidth(bw,packetNum);
		}
		mConnList.next();
	}
	mLock->unlock();
	return 0;	
}				   

								
int		
AosStreamSimuEntry::connNum()
{
	mLock->lock();
	int ret = mConnList.entries();
	mLock->unlock();
	return ret;
}
				   
bool	
AosStreamSimuEntry::modifyStream(const OmnIpAddr 						&recverIp,
							     const int		  						recverPort,
							     const int		  						bandwidth,
							     const int		  						packetSize,	                           
							     const AosStreamType::E 				streamType,
	                     	     const int                          	maxBw,
	                             const int                          	minBw)
{                                                                 
	bool connExist = false;                                       
	AosStreamSimuConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(recverIp,recverPort))
		{
			connExist = true;
			break;
		}
		mConnList.next();
	}
	
	if(connExist)
	{
		conn->modifyStream(bandwidth,
						   packetSize,
						   streamType,
						   maxBw,
						   minBw);
	}
	
	mLock->unlock();
	return connExist;	
}				     		  

bool
AosStreamSimuEntry::start()
{
	mStartTime = OmnTime::getCrtSec();
	return true;
}

bool	
AosStreamSimuEntry::stop()
{
	return true;
}	


bool
AosStreamSimuEntry::isFinished() const
{
	u32 crt = OmnTime::getCrtSec();
	return (crt >= mStartTime + mDuration);
}

