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
// 03/18/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/Testers/ReqProcTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"



bool AosXmlInterfaceReqProcTester::start()
{
	cout << "    Start XmlInterface Request Tester ...";
	basicTest();
	return true;
}


bool AosXmlInterfaceReqProcTester::basicTest()
{
	OmnIpAddr addr("127.0.0.1");
	OmnTcpClientPtr client = OmnNew OmnTcpClient("test", addr, 5555, 1, 
		eAosTLT_FirstWordHigh);
	OmnConnBuffPtr buff;

	OmnString errmsg;
	if (!client->connect(errmsg))
	{
		OmnAlarm << "Failed to connect: " << errmsg << enderr;
		return -1;
	}

	char len[2];
	len[0] = 0;
	len[1] = 5;
	client->writeToSock(len, 2);
	client->writeToSock("Linux", 5);

	int nread = client->smartRead(buff);
	OmnCreateTc << (nread >= 0) << endtc;
	OmnTrace << "Contents read: " << buff->getData() << endl;
	return true;
}


