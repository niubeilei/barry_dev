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
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestDrivers/TcpBouncer/TcpBouncer.h"

#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerClient.h"



void	
AosTcpBouncerTester::clientCreated(const AosTcpBouncerClientPtr &client)
{
	OmnTrace << "Client created: " << client->getSock() << endl;
}

// 
// This function is called when a client received something
// from the peer. The called can determine what to do with
// the receiving by returning "Action". 
//
AosTcpBouncer::Action 
AosTcpBouncerTester::msgReceived(
						const AosTcpBouncerClientPtr &client, 
						const OmnConnBuffPtr &buff)
{
	OmnTrace << "Message received: " << client->getSock() << endl;
	return AosTcpBouncer::eContinue;
}

//
// This function is called if the bouncer knows what to receive
// and the received is not what is expected. The called
// can control what to do by returning "Action".
//
AosTcpBouncer::Action 
AosTcpBouncerTester::recvFailed(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen)
{
	OmnTrace << "Receive Failed: " << client->getSock() << endl;
	return AosTcpBouncer::eAbort;
}

//
// This function is called right before sending (or bouncing)
// The data to be sent is 'data'. 
// The called can control the bouncer by returning "Action".
//
AosTcpBouncer::Action 
AosTcpBouncerTester::readyToBounce(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen)
{
	OmnTrace << "To bounce: " << client->getSock() << endl;
	return AosTcpBouncer::eContinue;
}

// 
// This function is called when the bouncing (sending) failed.
// The called can determine what to do by returning "Action".
//
AosTcpBouncer::Action 
AosTcpBouncerTester::bounceFailed(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen)
{
	OmnTrace << "Bounce Failed: " << client->getSock() << endl;
	return AosTcpBouncer::eAbort;
}

//
// This function is called when a client is closed, either
// it finished successfully or failed. The closing reason
// can be found by calling "getStatus()"
//
AosTcpBouncer::Action 
AosTcpBouncerTester::clientClosed(
						const AosTcpBouncerClientPtr &client)
{
	OmnTrace << "Client closed: " << client->getSock() << endl;
	return AosTcpBouncer::eContinue;
}

