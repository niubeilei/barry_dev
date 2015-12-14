////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TrafficGenCallback.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Ktcpvs/Tester/TrafficGenCallback.h"

#include "Debug/Debug.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/TcpTrafficGenClient.h"

#include "Ktcpvs/Tester/KtcpvsTester.h"


void		
AosTrafficGenCallback::msgRecved(const AosTcpTrafficGenClientPtr &client, 
								  const OmnConnBuffPtr &buff)
{
	OmnTrace << "Message received: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::connCreated(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn)
{
	OmnTrace << "Connection created: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::connClosed(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn)
{
	OmnTrace << "Connection closed: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::readyToSend(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen, 
								  bool &needToSend)
{
	OmnTrace << "Ready to send: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::sendFinished(const AosTcpTrafficGenClientPtr &client)
{
	OmnTrace << "Send finished: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::dataSent(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen)
{
	OmnTrace << "dataSent: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::sendFailed(const AosTcpTrafficGenClientPtr &client,
								  const char *data, 
								  const int dataLen,
								  const OmnRslt &rslt)
{
	OmnTrace << "Send failed: " << client->getClientId() << endl;
}


void		
AosTrafficGenCallback::recvFailed(const AosTcpTrafficGenClientPtr &client,
								  const OmnConnBuffPtr &buff, 
								  AosTcpTrafficGen::Action &action)
{
	OmnTrace << "Receive failed: " << client->getClientId() << endl;
}


void
AosTrafficGenCallback::trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients)
{
	mFinished = true;

	mTester->trafficGenFinished(clients);
	return;
}

