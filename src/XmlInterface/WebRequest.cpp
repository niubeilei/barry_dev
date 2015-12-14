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
// 05/11/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/WebRequest.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "TinyXml/TinyXml.h"
#include "UtilComm/TcpClient.h"


static OmnMutex sgLock;
static int sgTransId = 0;

AosWebRequest::AosWebRequest(const OmnTcpClientPtr &conn, const OmnConnBuffPtr &data)
:
AosTcpRequest(conn, data)
{
	sgLock.lock();
	mTransId = sgTransId++;
	sgLock.unlock();
}


AosWebRequest::~AosWebRequest()
{
}


AosNetReqType   
AosWebRequest::getType() const
{
	return eAosNetReqType_Web;
}


/*
OmnString       
AosWebRequest::getTargetId() const
{
	return "";
}


OmnString       
AosWebRequest::getOperator()
{
	return "";
}


OmnString       
AosWebRequest::getAppInstance()
{
	return "";
}


TiXmlNode *     
AosWebRequest::getContents()
{
	TiXmlElement *entry = mXmlDoc.RootElement();
	return entry;
}
*/


OmnTcpClientPtr 
AosWebRequest::getClient()
{
	return mConn;
}



