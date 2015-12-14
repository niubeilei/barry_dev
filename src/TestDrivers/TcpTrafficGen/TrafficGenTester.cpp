////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TrafficGenTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestDrivers/TcpTrafficGen/TrafficGenTester.h"

#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/TcpTrafficGenClient.h"


void		
AosTcpTrafficGenTester::msgRecved(const AosTcpTrafficGenClientPtr &client, 
								  const OmnConnBuffPtr &buff)
{
	OmnTrace << "Message received: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::connCreated(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn)
{
	OmnTrace << "Connection created: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::connClosed(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn)
{
	OmnTrace << "Connection closed: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::readyToSend(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen, 
								  bool &needToSend)
{
	OmnTrace << "Ready to send: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::sendFinished(const AosTcpTrafficGenClientPtr &client)
{
	OmnTrace << "Send finished: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::dataSent(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen)
{
	OmnTrace << "dataSent: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::sendFailed(const AosTcpTrafficGenClientPtr &client,
								  const char *data, 
								  const int dataLen,
								  const OmnRslt &rslt)
{
	OmnTrace << "Send failed: " << client->getClientId() << endl;
}


void		
AosTcpTrafficGenTester::recvFailed(const AosTcpTrafficGenClientPtr &client,
								  const OmnConnBuffPtr &buff, 
								  AosTcpTrafficGen::Action &action)
{
	OmnTrace << "Receive failed: " << client->getClientId() << endl;
}


void
AosTcpTrafficGenTester::trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients)
{
	OmnTrace << "TrafficGen finished: " << clients.entries() << endl;
	return;
}

