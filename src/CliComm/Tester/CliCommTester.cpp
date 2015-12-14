////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliComm/Tester/CliCommTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


bool AosCliCommTester::start()
{
	// 
	// Test default constructor
	//
	testCli();
	return true;
}


bool AosCliCommTester::testCli()
{
	OmnBeginTest << "Test AppProxy CLI";
	mTcNameRoot = "Test_CLI";

	OmnTcpClient client("cli", OmnIpAddr("127.0.0.1"), 5500, 1, 
		OmnTcp::eNoLengthIndicator);
	OmnString errmsg;
	bool c = client.connect(errmsg);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(c)) << errmsg << endtc;

	OmnString cmd = 
		"<CliCommRequest>"
			"<RequestId>100</RequestId>"
			"<Command>"
				"app proxy add app1 http 192.168.1.81 5000 ssl"
			"</Command>"
		"</CliCommRequest>";
	client.writeTo(cmd.data(), cmd.length());
	
	OmnConnBuffPtr buff;
	bool connbroken;
	c = client.readFrom1(buff, connbroken);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(c)) << errmsg << endtc;
	if (c)
	{
		// 
		// Read the response.
		//
		cout << "Got the response: " << buff->getBuffer() << endl;
	}

	return true;
}


