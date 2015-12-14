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
// 02/15/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/TcpClientTester/TcpClientTester.h"

#include "UtilComm/TcpServerEpoll.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Porting/Sleep.h"
#include "SEClient/TcpClientTester/TcpClientThrd.h"
#include "SEClient/TcpClientTester/Ptrs.h"

#include "aosUtil/Memory.h"
static int sgNumThread = 10;


bool AosTcpClientTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosTcpClientTester::basicTest()
{
	OmnBeginTest << "Test AosTcpClient";
	OmnString addr = "192.168.99.96";
	int port = 5533;
	for(int i=0; i<sgNumThread; i++)
	{
		OmnString name;
		name << "client====" << i;
		AosTcpClientThrdPtr thrd = OmnNew AosTcpClientThrd(name, addr, port);
	}
	OmnSleep(100000);
	return true;
}


