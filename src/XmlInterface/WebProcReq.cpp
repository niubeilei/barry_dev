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
// 05/27/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/WebProcReq.h"

#include "alarm_c/alarm.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


void			
AosWebProcReq::sendResponse(const OmnString &response)
{
	aos_assert(mConn);
	mConn->smartSend(response.data(), response.length());
}


char *
AosWebProcReq::getData() const
{
	return mData->getData();
}
