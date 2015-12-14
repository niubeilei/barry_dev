////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgRecver.h
// Description:
//	This is an interface for Message Receiver. If one wants to receive
//  some messages, it should implement this interface and register
//  with a message reader, such as OmnInfobusClt.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_MsgRecver_h
#define Omn_Message_MsgRecver_h

#include "Message/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"



class OmnMsgRecver : virtual public OmnRCObject
{
public:
	//
	// When a communicator receives a message and matches this receiver,
	// it calls this function to pass the received message over. If the
	// receiver consumed the message, it returns true. The communicator
	// shall not try other receivers. If it returns false, the 
	// communicator may try others. Normally it should return true.
	//
	virtual bool		msgReceived(const OmnMsgPtr &msg, void *userData) = 0;
	virtual OmnString	getRecvName() const = 0;
	virtual void		ttlExpired() const = 0;
};
#endif
