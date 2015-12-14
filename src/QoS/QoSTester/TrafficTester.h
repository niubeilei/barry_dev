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
// 01/10/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_QoS_QoSTester_TrafficTester_h
#define Omn_QoS_QoSTester_TrafficTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/IpAddr.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosQosTrafficTester : public OmnTestPkg
{
private:
	struct Conn
	{
	};

	AosTcpBouncerPtr	mBouncer;
	OmnIpAddr			mLocalIP;
	u16					mLocalPort;
	u32					mNumLocalPorts;
	u32					mMaxConns;
	u32					mBounceType;

public:
	AosQosTrafficTester();
	~AosQosTrafficTester() {}

	virtual bool		start();

private:
//	bool	basicTest();
//	bool	startBouncer();
};

#endif

