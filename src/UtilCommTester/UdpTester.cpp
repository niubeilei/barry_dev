////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilCommTester/UdpTester.h"

#include "Debug/Debug.h"
#include "Network/NetIf.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "UtilComm/UdpComm.h"


bool OmnUdpTester::start()
{
	OmnUdpComm udp(OmnIpAddr("192.168.0.102"), 5000, "Test1");
	if (!udp.isConnGood())
	{
		OmnAlarm << "Connection not good!" << enderr;
		return false;
	}

	const int length = 10000;
	char buff[length];
	memset(buff, 'c', length);
	OmnRslt rslt = udp.sendTo(buff, length, OmnIpAddr("192.168.0.68"), 5000); 
	if (!rslt)
	{
		OmnAlarm << "Failed to send" << enderr;
	}
	else
	{
		OmnTrace << "packet sent: " << length << endl;
	}
	return true;
}



