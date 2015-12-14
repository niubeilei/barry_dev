////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpMsgReader.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpMsgReader_h
#define Omn_UtilComm_TcpMsgReader_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"


class OmnTcpMsgReader : public virtual OmnRCObject
{
public:
	virtual OmnString	getTcpMsgReaderName() const = 0;
	virtual int			nextMsg(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn) = 0;
	virtual bool		acceptConn(const int sock,const OmnIpAddr &ipAddr,const int port)
	{
		return true;
	}

	virtual void		connAccepted(const OmnTcpClientPtr &conn)
	{
		return;
	}
	
	virtual void		connClosed(const OmnTcpClientPtr &conn)
	{
		return;
	}

};
#endif
