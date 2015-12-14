////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilCommTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilCommTester/UtilCommTestSuite.h"

#include "Debug/Debug.h"
#include "Network/NetIf.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "UtilCommTester/StreamerTester.h"
#include "UtilCommTester/StreamerServerTester.h"
#include "UtilCommTester/UdpTester.h"
#include "UtilComm/Streamer.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/Streamer.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/StreamRequester.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/StreamRecvTrans.h"
#include "UtilComm/SmStreamInvite.h"
#include "UtilComm/SmStreamInviteResp.h"
#include "UtilComm/SmStreamAck.h"
#include "UtilComm/SmStreamAckResp.h"


OmnUtilCommTestSuite::OmnUtilCommTestSuite()
{
}


OmnTestSuitePtr		
OmnUtilCommTestSuite::getSuite(const int argc, char **argv)
{
	int index = 0;
	bool isServer = false;
	while (index < argc)
	{
		OmnString str(argv[index]);
		str.toLower();
		if (str == "-server")
		{
			isServer = true;
			break;
		}
		index++;
	}

	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UtilCommTestSuite", "UtilComm Library Test Suite");

	// 
	// Now add all the testers
	//
/*
	if (isServer)
	{
		suite->addTestPkg(OmnNew OmnStreamerServerTester());
	}
	else
	{
		suite->addTestPkg(OmnNew OmnStreamerTester());
	}
*/
	suite->addTestPkg(OmnNew OmnUdpTester());

	return suite;
}
