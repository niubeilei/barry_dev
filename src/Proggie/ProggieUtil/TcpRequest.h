////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 03/23/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Proggie_ProggieUtil_TcpRequest_h
#define Aos_Proggie_ProggieUtil_TcpRequest_h

#include "Proggie/ProggieUtil/NetRequest.h"
#include "UtilComm/TcpClient.h"


class OmnString;

class AosTcpRequest : public AosNetRequest
{
	OmnDefineRCObject;

protected:
	OmnTcpClientPtr 	mConn;

public:
	AosTcpRequest(const OmnTcpClientPtr &conn, const OmnConnBuffPtr &data)
	:
	AosNetRequest(data),
	mConn(conn)
	{
	}

	virtual OmnTcpClientPtr	getConn() {return mConn;}
	virtual void	closeConn();
	virtual void	sendResponse(const OmnString &response);
};

#endif

