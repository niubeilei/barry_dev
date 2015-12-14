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
#include "Proggie/ProggieUtil/NetRequest.h"

#include "alarm_c/alarm.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"



char *
AosNetRequest::getData() const
{
	return mData->getData();
}


// Chen Ding, 2013/03/07
OmnConnBuffPtr
AosNetRequest::getConnData() const
{
	return mData;
}


int
AosNetRequest::getDataLen() const
{
	return mData->getDataLength();
}


OmnConnBuffPtr
AosNetRequest::getDataBuff() const
{
	return mData;
}

