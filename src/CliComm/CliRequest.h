////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliRequest.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliComm_CliRequest_h
#define Aos_CliComm_CliRequest_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"


class AosCliRequest : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eBuffLen = 10000,
		eRequestMinLen = 75,
		eHeadTagLen = 16
	};

	enum ReturnCode
	{
		e200Ok = 200,
		eDataTooLong = 301,
		eCmdError = 302,
		eSyntaxError = 303,
	};

private:
	OmnTcpClientPtr	mTcpClient;
	char			mBuff[eBuffLen];
	unsigned int	mBuffLen;
	OmnString		mRequestId;
	OmnString		mCmd;

public:
	AosCliRequest(const OmnTcpClientPtr &client);
	virtual ~AosCliRequest() {}

	bool	procMsg(const OmnConnBuffPtr &buff);

private:
	bool	sendResponse(const ReturnCode code, const OmnString &rslt);
	bool	parseRequest();
};

#endif

