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
#include "UtilComm/Tester/TcpServerTester.h"

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

#include "aosUtil/Memory.h"


AosTcpServerTester::AosTcpServerTester()
{
	mName = "AosTcpServerTester";
}


AosTcpServerTester::~AosTcpServerTester()
{
}


bool 
AosTcpServerTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool 
AosTcpServerTester::basicTest()
{
	OmnBeginTest << "Test AosTcpServer";
	mTcNameRoot = "TcpServerTest";
	
	mServer = OmnNew OmnTcpServerEpoll("test.sock", "test", eAosTLT_FirstFourHigh);
	OmnTcpListenerPtr thisptr(this, false);
	mServer->setListener(thisptr);
	mServer->startReading();
	return true;
}

void 
AosTcpServerTester::msgRecved(const OmnConnBuffPtr &buff, const OmnTcpClientPtr &conn)
{
	conn->smartSend(buff->getData(), buff->getDataLength());
	// cout << buff->getData() << endl;
}

