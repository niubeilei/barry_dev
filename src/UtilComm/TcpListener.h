////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpListener_h
#define Omn_UtilComm_TcpListener_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class OmnTcpListener : public virtual OmnRCObject
{
public:
	virtual OmnString	getTcpListenerName() const = 0;
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn) = 0;
	virtual void		connCreated(const OmnTcpClientPtr &conn) = 0;
	virtual void		connClosed(const OmnTcpClientPtr &client) = 0;
};
#endif
