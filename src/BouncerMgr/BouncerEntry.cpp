////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BouncerEntry.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "BouncerMgr/BouncerEntry.h"

#include "BouncerMgr/BouncerConn.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerClient.h"


AosBouncerEntry::AosBouncerEntry(const OmnIpAddr	&recvIp,
								 const int		&recvPort,
								 const OmnString &protocol)
:
mLock(OmnNew OmnMutex()),
mRecverIp(recvIp),
mRecverPort(recvPort),
mProtocol(protocol)
{
}								 

AosBouncerEntry::AosBouncerEntry()
:
mLock(OmnNew OmnMutex()),
mRecverPort(-1)
{
}
	
AosBouncerEntry::~AosBouncerEntry()
{
}

bool	
AosBouncerEntry::entryExist(const OmnIpAddr 		&recverIp,
					   		const int		  		&recverPort,
					   		const OmnString 		&protocol)
{
	return ((mRecverIp == recverIp) && 
			(mRecverPort == recverPort) && 
			(mProtocol == protocol));
}

bool	
AosBouncerEntry::addConn(const OmnIpAddr 		&senderIp,
						 const int		  		&senderPort)
{
	bool connExist = false;
	AosBouncerConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(senderIp,senderPort))
		{
			connExist = true;
			break;
		}
		mConnList.next();
	}
	
	if(!connExist)
	{
		conn = OmnNew AosBouncerConn(senderIp,
									 senderPort,
									 mRecverIp,
									 mRecverPort,
									 mProtocol);
		mConnList.append(conn);
	}	
	mLock->unlock();
	return !connExist;	
}
						 				
bool	
AosBouncerEntry::removeConn(const OmnIpAddr 		&senderIp,
				   			const int		  		&senderPort)
{
	bool connExist = false;
	AosBouncerConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(senderIp,senderPort))
		{
			connExist = true;
			mConnList.eraseCrt();			
			break;
		}
		mConnList.next();
	}
	mLock->unlock();
	return connExist;	
}				   
								
int		
AosBouncerEntry::connNum()
{
	mLock->lock();
	int ret = mConnList.entries();
	mLock->unlock();
	return ret;
}
				   
bool	
AosBouncerEntry::getBandwidth(const OmnIpAddr 		&senderIp,
				     		  const int		  		&senderPort,
				     		  int		  			&curBandwidth,
				     		  int		  			&avgBandwidth)
{
	bool connExist = false;
	AosBouncerConnPtr conn;
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(senderIp,senderPort))
		{
			connExist = true;
			break;
		}
		mConnList.next();
	}
	
	if(connExist)
	{
		conn->getBandwidth(curBandwidth,avgBandwidth);
	}
	
	mLock->unlock();
	return connExist;	
}				     		  

bool	
AosBouncerEntry::resetConnStat(const OmnIpAddr 		&senderIp,
				   	  		   const int		  	&senderPort)
{
	bool connExist = false;
	AosBouncerConnPtr conn;
	
	mLock->lock();
	mConnList.reset();
	while(mConnList.hasMore())
	{
		conn = mConnList.crtValue();
		if(conn->connExist(senderIp,senderPort))
		{
			connExist = true;
			break;
		}
		mConnList.next();
	}

	if(connExist)
	{
		conn->resetStat();
	}
	mLock->unlock();
	return connExist;	
	
}

bool
AosBouncerEntry::start()
{
	return true;
}

bool	
AosBouncerEntry::stop()
{
	return true;
}	

