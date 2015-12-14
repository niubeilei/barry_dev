////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncerListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_TcpBouncerListener_h
#define Omn_UtilComm_TcpBouncerListener_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/TcpBouncer.h"
#include "UtilComm/Ptrs.h"


class OmnRslt;
class AosTcpBouncerListener : public virtual OmnRCObject
{
public:
	virtual void	clientCreated(const AosTcpBouncerClientPtr &client) = 0;

	// 
	// This function is called when a client received something
	// from the peer. The called can determine what to do with
	// the receiving by returning "Action". 
	//
	virtual AosTcpBouncer::Action msgReceived(
						const AosTcpBouncerClientPtr &client, 
						const OmnConnBuffPtr &buff) = 0;

	//
	// This function is called if the bouncer knows what to receive
	// and the received is not what is expected. The called
	// can control what to do by returning "Action".
	//
	virtual AosTcpBouncer::Action recvFailed(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen) = 0;

	//
	// This function is called right before sending (or bouncing)
	// The data to be sent is 'data'. 
	// The called can control the bouncer by returning "Action".
	//
	virtual AosTcpBouncer::Action readyToBounce(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen) = 0;

	// 
	// This function is called when the bouncing (sending) failed.
	// The called can determine what to do by returning "Action".
	//
	virtual AosTcpBouncer::Action bounceFailed(
						const AosTcpBouncerClientPtr &client, 
						char *data, 
						u32 dataLen) = 0;

	//
	// This function is called when a client is closed, either
	// it finished successfully or failed. The closing reason
	// can be found by calling "getStatus()"
	//
	virtual AosTcpBouncer::Action clientClosed(
						const AosTcpBouncerClientPtr &client) = 0;
};
#endif

