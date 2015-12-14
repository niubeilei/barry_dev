////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Addr.h
// Description:
//	Addr contains a host name, IP address, and a port. It is used 
//	by protocols such as SIP.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Addr_h
#define Omn_Util_Addr_h

#include "Util/String.h"
#include "Util/IpAddr.h"

class OmnAddr
{
private:
	int			mHostStart;
	int			mHostLength;
	OmnString	mHost;

	OmnIpAddr	mIpAddr;
	
	int			mPortStart;
	int			mPortLength;
	int			mPort;	
	
public:
	OmnAddr()
	:
	mHostStart(-1),
	mHostLength(-1),
	mPortStart(-1),
	mPortLength(-1),
	mPort(-1)
	{
	}

	~OmnAddr();
};
#endif

