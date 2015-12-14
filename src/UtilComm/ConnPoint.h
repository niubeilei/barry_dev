////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ConnPoint.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_ConnPoint_h
#define Omn_UtilComm_ConnPoint_h

#include "Util/IpAddr.h"



class OmnConnPoint OmnDeriveFromObj
{
public:
	OmnIpAddr		mIpAddr;
	int				mPort;

	OmnConnPoint()
		:
	mPort(-1)
	{
	}


	OmnConnPoint(const OmnIpAddr &ipAddr, const int port)
		:
	mIpAddr(ipAddr),
	mPort(port)
	{
	}

	OmnIpAddr		getIpAddr() const {return mIpAddr;}
	int				getPort() const {return mPort;}
};

#endif
