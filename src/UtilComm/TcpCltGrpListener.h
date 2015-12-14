////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCltGrpListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpCltGrpListener_h
#define Omn_UtilComm_TcpCltGrpListener_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class OmnTcpCltGrpListener : public virtual OmnRCObject
{
public:
	virtual OmnString	getTcpListenerName() const = 0;
	virtual void		msgRecved(const OmnTcpCltGrpPtr &group, 
								const OmnConnBuffPtr &buff,
								const OmnTcpClientPtr &conn) = 0;
	virtual void		newConn(const OmnTcpClientPtr &conn) = 0;
	virtual bool		connClosed(const OmnTcpCltGrpPtr &, 
								const OmnTcpClientPtr &client) = 0;
};
#endif
