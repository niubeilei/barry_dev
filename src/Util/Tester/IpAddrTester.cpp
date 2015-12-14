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
// 02/24/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/IpAddrTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

OmnIpAddrTester::OmnIpAddrTester()
{
	mName = "OmnIpAddrTester";
	mTries = 100;
}


bool OmnIpAddrTester::start()
{
	OmnIpAddr addr1("192.168.1.2");
	cout << "addr1 = " << addr1.toString().data() << endl;


	testIsValidMask();
	return true;
}


static OmnString sgMasks[] = 
{
	"255.255.255.255", 
	"255.255.255.254", 
	"255.255.255.252", 
	"255.255.255.248", 
	"255.255.255.240", 
	"255.255.255.224", 
	"255.255.255.192", 
	"255.255.255.128", 

	"255.255.255.0", 
	"255.255.254.0", 
	"255.255.252.0", 
	"255.255.248.0", 
	"255.255.240.0", 
	"255.255.224.0", 
	"255.255.192.0", 
	"255.255.128.0", 

	"255.255.0.0", 
	"255.254.0.0", 
	"255.252.0.0", 
	"255.248.0.0", 
	"255.240.0.0", 
	"255.224.0.0", 
	"255.192.0.0", 
	"255.128.0.0", 

	"255.0.0.0", 
	"254.0.0.0", 
	"252.0.0.0", 
	"248.0.0.0", 
	"240.0.0.0", 
	"224.0.0.0", 
	"192.0.0.0", 
	"128.0.0.0"
};


bool
OmnIpAddrTester::testIsValidMask()
{
	u32 a = 0xffffffff;
	for (int i=0; i<32; i++)
	{
		if (i == 32) a = 0;
		OmnIpAddr addr(htonl(a));
		OmnTC(OmnExpected<bool>(true), 
				OmnActual<bool>(OmnIpAddr::isValidMask(addr))) 
			<< addr.toString() << endtc;
		a <<= 1;

		OmnIpAddr addr1(sgMasks[i]);
		OmnTC(OmnExpected<bool>(true), 
				OmnActual<bool>(OmnIpAddr::isValidMask(addr1))) 
			<< addr1.toString() << endtc;
	}

	// 
	// Try random ones
	//
	// for (int i=0; i<mTries; i++)
	// {
	// 	OmnString addr = aos_next_ip_str()
	// }
	return true;
}


