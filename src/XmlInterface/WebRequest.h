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
#ifndef Aos_XmlInterface_Server_WebRequest_h
#define Aos_XmlInterface_Server_WebRequest_h

#include "Proggie/ProggieUtil/TcpRequest.h"
#include "TinyXml/TinyXml.h"


class AosWebRequest : public AosTcpRequest
{
private:
	TiXmlDocument	mXmlDoc;
	int				mTransId;

public:
	AosWebRequest(const OmnTcpClientPtr &conn, const OmnConnBuffPtr &data);
	~AosWebRequest();

	virtual AosNetReqType   getType() const;

	/*
 	virtual OmnString       getTargetId() const;
 	virtual OmnString       getOperator();
 	virtual OmnString       getAppInstance();
 	virtual TiXmlNode *     getContents();
	*/

	virtual OmnTcpClientPtr getClient();

	int		getTransId() const {return mTransId;}
};
#endif

