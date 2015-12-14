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
#ifndef Aos_Proggie_ProggieUtil_NetRequest_h
#define Aos_Proggie_ProggieUtil_NetRequest_h

// #include "LogServer/LogDataTypes.h"
#include "Proggie/ProggieUtil/ProggieUtilTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"


class OmnString;
class TiXmlNode;

class AosNetRequest : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnConnBuffPtr		mData;

public:
	AosNetRequest()
	{
	}

	AosNetRequest(const OmnConnBuffPtr &data)
	:
	mData(data)
	{
	}

	char *	getData() const;
	int		getDataLen() const;
	OmnConnBuffPtr	getDataBuff() const;
	virtual void	sendResponse(const OmnString &response) = 0;
	virtual AosNetReqType	getType() const = 0;

	/* Chen Ding, 2015/01/14
	virtual OmnString		getTargetId() const = 0;
	virtual OmnString		getOperator() = 0;
	virtual OmnString       getAppInstance() = 0;
	virtual TiXmlNode *		getContents() = 0;
	virtual OmnTcpClientPtr getClient() = 0;
	virtual void			closeConn() = 0;
	*/
	OmnConnBuffPtr getConnData() const;		// Chen Ding, 2013/03/07
};

#endif

