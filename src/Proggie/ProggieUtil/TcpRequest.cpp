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
// 	This class is used as the generic network-bound requests. Each request
// 	has a OmnConnBuff and a connection.
//   
//
// Modification History:
// 05/27/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/ProggieUtil/TcpRequest.h"

#include "alarm_c/alarm.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


void			
AosTcpRequest::sendResponse(const OmnString &response)
{
	aos_assert(mConn);
	mConn->smartSend(response.data(), response.length());
}


void	
AosTcpRequest::closeConn()
{
	if (mConn) mConn->closeConn();
}

