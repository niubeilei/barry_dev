////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Conn.h
// Description:
//	A connection can be a TCP or UDP connection. It is used by OmnComm
//  to actually send and receive messages. 
//  This class defines the interface.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_Conn_h
#define Omn_UtilComm_Conn_h

#include "Debug/ErrId.h"
#include "Debug/Rslt.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"

class OmnIpAddr;



class OmnConn : public OmnRCObject
{
public:
	enum ConnType
	{
		eTcpClient,
		eTcpServer,
		eUdp
	};

public:
	virtual OmnString	getName() const = 0;

	//
	// Queries
	//
	virtual bool		isConnGood() const = 0;
	static  bool		isConnGood(const int sock);
	virtual bool		closeConn() = 0;
	virtual OmnRslt		connect(OmnString &err) = 0;

	virtual ConnType	getConnType() const = 0;
	virtual OmnString	toString() const = 0;

	virtual int			getSock() const = 0;
	virtual int			getLocalPort() const = 0;

	virtual	OmnIpAddr	getLocalIpAddr() const = 0;
	virtual void		setDebug(const bool flag) = 0;

	virtual bool		hasPacketToRead() = 0;
};

#endif
