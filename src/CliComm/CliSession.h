////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliSession.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliComm_CliSession_h
#define Aos_CliComm_CliSession_h

#include "CliComm/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"


class AosCliSession : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnTcpClientPtr		mClient;
	AosCliRequestPtr	mRequest;

public:
	AosCliSession(const OmnTcpClientPtr &client);
	virtual ~AosCliSession() {}

	int 	getSock() const;
	bool	msgRecved(const OmnTcpClientPtr &client,
					  const OmnConnBuffPtr &buff);
};

#endif

