////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommClt.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliComm_CliCommClt_h
#define Aos_CliComm_CliCommClt_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"


class AosCliCommClt : public OmnRCObject
{
private:
	OmnTcpClientPtr		mClient;

public:
	AosCliCommClt(const OmnString &name,
		const OmnIpAddr &remoteAddr,
		const int remotePort);
	virtual ~AosCliCommClt() {}

	bool read(OmnConnBuffPtr &buff, bool &broken);
};

#endif

