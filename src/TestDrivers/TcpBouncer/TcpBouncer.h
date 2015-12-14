////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestDrivers_Bouncer_h
#define Omn_TestDrivers_Bouncer_h

#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpBouncerListener.h"


class AosTcpBouncerTester : public AosTcpBouncerListener
{
	OmnDefineRCObject;

public:
	virtual void	clientCreated(const AosTcpBouncerClientPtr &client);

	// 
	// This function is called when a client received something
	// from the peer. The called can determine what to do with
	// the receiving by returning "Action". 
	//
	virtual AosTcpBouncer::Action msgReceived(
						const AosTcpBouncerClientPtr &client, 
						const OmnConnBuffPtr &buff);

	//
	// This function is called if the bouncer knows what to receive
	// and the received is not what is expected. The called
	// can control what to do by returning "Action".
	//
	virtual AosTcpBouncer::Action recvFailed(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen);

	//
	// This function is called right before sending (or bouncing)
	// The data to be sent is 'data'. 
	// The called can control the bouncer by returning "Action".
	//
	virtual AosTcpBouncer::Action readyToBounce(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen);

	// 
	// This function is called when the bouncing (sending) failed.
	// The called can determine what to do by returning "Action".
	//
	virtual AosTcpBouncer::Action bounceFailed(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen);

	//
	// This function is called when a client is closed, either
	// it finished successfully or failed. The closing reason
	// can be found by calling "getStatus()"
	//
	virtual AosTcpBouncer::Action clientClosed(
						const AosTcpBouncerClientPtr &client);
};
#endif

