////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/TrafficGenThread.h"


#include "QoS/QoSTester/QosTestSuite.h"
#include "QoS/QoSTester/ChannelTester.h"
#include "QoS/QoSTester/ChannelMonitorTester.h"
#include "QoS/QoSTester/BandwidthManagerTester.h"
#include "QoS/QoSTester/TrafficTester.h"

OmnTestSuitePtr		
AosQoSTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("QosTestSuite", "Qos Test Suite");


	// 
	// Now add all the testers
	//
    //suite->addTestPkg(OmnNew AosQoSChannelTester());
 //   suite->addTestPkg(OmnNew AosQoSBandwidthManagerTester());
     suite->addTestPkg(OmnNew AosQoSChannelMonitorManagerTester());
    //suite->addTestPkg(OmnNew AosQosTrafficTester());
 
	return suite;
}

