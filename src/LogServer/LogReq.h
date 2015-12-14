//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 03/20/2009: Created by Sharon Shen
// 12/31/2012: Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_LogServer_LogReq_h
#define AOS_LogServer_LogReq_h

#include "LogServer/LogDataTypes.h"
#include "Proggie/ProggieUtil/NetRequest.h"
#include "Util/String.h"
#include "./TinyXml/TinyXml.h"
#include "UtilComm/TcpClient.h"

class AosLogReq : public AosNetRequest
{
private:
	TiXmlDocument   mXmlDoc;	

public:
	AosLogReq();
	AosLogReq(const char *buff);
	~AosLogReq();
	
private:
	// AosNetRequest Interface
	virtual void    sendResponse(const OmnString &response);
    virtual AosNetReqType   getType() const;

	virtual OmnString   	getTargetId() const;
	virtual OmnString   	getOperator();
	virtual TiXmlElement  * getContents();
	virtual OmnString       getAppInstance();

};
#endif
#endif
