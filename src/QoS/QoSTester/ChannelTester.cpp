////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "QoS/QoSTester/ChannelTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"

static OmnTcpClientPtr sgTcp[128];

bool AosQoSChannelTester::start()
{
	unsigned int ret;
	// 
	// Test default constructor
	//

	basicTest();
	return true;
}

bool AosQoSChannelTester::basicTest()
{
	OmnBeginTest << "Test SSL Server";
	mTcNameRoot = "Test_QoSChannel";
	OmnIpAddr addr("0.0.0.0");
	u16 port = 5000;

	// 
	// Create a TCP connection
	// 
	OmnTcpServerGrp tcp(addr, port, 1, "QoSChannel", 1000, OmnTcp::eNoLengthIndicator);
    OmnTcpCltGrpListenerPtr thisPtr(this, false);
    tcp.setListener(thisPtr);
	tcp.startReading();

	while (1)
	{
		OmnSleep(1);
	}

	return true;
}	

void
AosQoSChannelTester::msgRecved(const OmnTcpCltGrpPtr &group,
                              const OmnConnBuffPtr &buff,
                              const OmnTcpClientPtr &tcp)
{
	// 
	// Retrieve the conn
	//
	return;
}	

void        
AosQoSChannelTester::newConn(const OmnTcpClientPtr& tcp)
{
	int i,ret;
	OmnString errmsg;

}


bool        
AosQoSChannelTester::connClosed(
			const OmnTcpCltGrpPtr &,
            const OmnTcpClientPtr &client)
{
	return true;
}


