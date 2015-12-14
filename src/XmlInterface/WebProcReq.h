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
#ifndef Aos_XmlInterface_WebProcReq_h
#define Aos_XmlInterface_WebProcReq_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"


class OmnString;

class AosWebProcReq : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnTcpClientPtr 	mConn;
	OmnConnBuffPtr		mData;

public:
	AosWebProcReq(const OmnTcpClientPtr &conn, const OmnConnBuffPtr &data)
	:
	mConn(conn),
	mData(data)
	{
	}

	char *	getData() const;
	void	sendResponse(const OmnString &response);
};

#endif

