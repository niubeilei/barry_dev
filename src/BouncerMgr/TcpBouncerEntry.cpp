////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncerEntry.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "BouncerMgr/TcpBouncerEntry.h"

#include "BouncerMgr/BouncerConn.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpServer.h"

AosTcpBouncerEntry::AosTcpBouncerEntry(const OmnIpAddr& senderIp, 
									   const int& senderPort)
:
AosBouncerEntry::AosBouncerEntry(senderIp,senderPort,"tcp")
{
}
	
AosTcpBouncerEntry::~AosTcpBouncerEntry()
{
}	

bool	
AosTcpBouncerEntry::start()
{
	mTcpServer = OmnNew OmnTcpServer(mRecverIp,mRecverPort,1,"TcpBouncer");

    OmnTcpListenerPtr thisPtr(this, false);
    mTcpServer->setListener(thisPtr);
	
	OmnString strErr;
	if(!mTcpServer->connect(strErr))
	{

cout  << mRecverIp.toString() << ":" << mRecverPort << ":" << strErr << endl;
		return false;
	}
	
	mTcpServer->startReading();	
	return true;
}

bool	
AosTcpBouncerEntry::stop()
{
	// tbd
	// need to stop reading
	mTcpServer->stopReading();
	//	mTcpServer->closeConn();
	return true;
}

OmnString   
AosTcpBouncerEntry::getTcpListenerName() const
{
	return "TcpBouncerEntry";
}

void        
AosTcpBouncerEntry::msgRecved(const OmnConnBuffPtr &buff,
          					  const OmnTcpClientPtr &conn)
{
	bool found = false;
		
	OmnIpAddr senderIp = buff->getRemoteAddr();
	int senderPort = buff->getRemotePort();
cout << senderIp.toString() <<":" <<  senderPort << endl;
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
			bouncerConn->getPacket(buff,"tcp");
			found = true;
			break;
		}
		mConnList.next();
	}
	if(!found)
	{
cout << "can not find entry" <<senderIp.toString() << ":" << senderPort << endl;
	}
	mLock->unlock();
}

