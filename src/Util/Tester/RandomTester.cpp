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
// 02/22/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/RandomTester.h"

#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Random/random_util.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

OmnRandomTester::OmnRandomTester()
{
	mName = "OmnRandomTester";
	mTries = 100000;
	mTestNextIntTries = 100;
	mTestNextIncorrectIpTries = 100;
	mTestNextIpTries = 100000;
}


bool OmnRandomTester::start()
{
	u32 tries = 100;
	// testNextInt();
	// testNextIncorrectIp();
	// testNextIP();
	// testNextMask();
	// testNextTimeStr(tries);
	testNextInvalidTimeStr(tries);
	
	// OmnIpAddr a1("192.168.1.129");
	// OmnIpAddr m1("255.255.255.240");
	// u32 rr = htonl(a1.getIPv4()) & htonl(m1.getIPv4());
	// u32 dd4 = htonl(~(m1.getIPv4()))+1;
	// for (int i=0; i<dd4; i++)
	// {
	// 	// OmnIpAddr aaa(htonl(htonl(rr) + i));
	// 	OmnIpAddr aaa(htonl(rr + i));
	// 	cout << aaa.toString().data() << endl;
	// 	OmnIpAddr addr = aos_next_ip(a1, m1);
	// 	cout << addr.toString().data() << endl;
	// }

	return true;
}


bool
OmnRandomTester::testNextInt()
{
	for (int i=0; i<mTestNextIntTries; i++)
	{
		int min = rand();
		int max = rand();
		if ((rand() & 0xff) == 1) min = -min;
		if ((rand() & 0xff) == 1) max = -max;
		if (min > max)
		{
			OmnTC(OmnExpected<bool>(true), 
				OmnActual<bool>(aos_next_int(min, max) == min)) << endtc;
		}
		else
		{
			for (int k=0; k<10; k++)
			{
				int value = aos_next_int(min, max);
				bool ret = value >= min && value <= max;
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) 
					<< "value = " << value << ". min = " << min 
					<< ". max = " << max << endtc;
			}
		}
	}

	return true;
}


bool
OmnRandomTester::testNextIncorrectIp()
{
	// Commented out by Chen Ding, 04/11/2012
	// OmnString str;
	// for (int i=0; i<mTestNextIncorrectIpTries; i++)
	// {
	// 	str = aos_next_incorrect_ip();
	// 	OmnTC(OmnExpected<bool>(false), OmnActual<bool>(OmnIpAddr::isValidAddr(str))) << endtc;
	// }

	return true;
}


bool
OmnRandomTester::testNextIP()
{
	OmnIpAddr a;
	OmnString str;

	for (int i=0; i<mTestNextIpTries; i++)
	{
		OmnIpAddr addr(aos_next_int(0, INT_MAX));
		OmnIpAddr mask(aos_next_mask());

		for (int k=0; k<1; k++)
		{
			a = aos_next_ip(addr, mask);
			str = aos_next_ip_str(addr, mask);
			OmnIpAddr aa(str);

			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(OmnIpAddr::isAddrIn(a, addr, mask))) 
					<< "Addr: " << a.toString() 
					<< ". Netaddr: " << addr.toString() 
					<< ". Netmask: " << mask.toString() << endtc;
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(OmnIpAddr::isAddrIn(aa, addr, mask))) 
					<< "Addr: " << aa.toString()	
					<< ". Netaddr: " << addr.toString() 
					<< ". Netmask: " << mask.toString() << endtc;
		}
	}

	return true;
}


bool
OmnRandomTester::testNextMask()
{
	OmnIpAddr masks[33];

	masks[0]= OmnIpAddr("255.255.255.255");
	masks[1]= OmnIpAddr("255.255.255.254");
	masks[2]= OmnIpAddr("255.255.255.252");
	masks[3]= OmnIpAddr("255.255.255.248");
	masks[4]= OmnIpAddr("255.255.255.240");
	masks[5]= OmnIpAddr("255.255.255.224");
	masks[6]= OmnIpAddr("255.255.255.192");
	masks[7]= OmnIpAddr("255.255.255.128");
	masks[8]= OmnIpAddr("255.255.255.0");
	masks[9]= OmnIpAddr("255.255.254.0");
	masks[10]= OmnIpAddr("255.255.252.0");
	masks[11]= OmnIpAddr("255.255.248.0");
	masks[12]= OmnIpAddr("255.255.240.0");
	masks[13]= OmnIpAddr("255.255.224.0");
	masks[14]= OmnIpAddr("255.255.192.0");
	masks[15]= OmnIpAddr("255.255.128.0");
	masks[16]= OmnIpAddr("255.255.0.0");
	masks[17]= OmnIpAddr("255.254.0.0");
	masks[18]= OmnIpAddr("255.252.0.0");
	masks[19]= OmnIpAddr("255.248.0.0");
	masks[20]= OmnIpAddr("255.240.0.0");
	masks[21]= OmnIpAddr("255.224.0.0");
	masks[22]= OmnIpAddr("255.192.0.0");
	masks[23]= OmnIpAddr("255.128.0.0");
	masks[24]= OmnIpAddr("255.0.0.0");
	masks[25]= OmnIpAddr("254.0.0.0");
	masks[26]= OmnIpAddr("252.0.0.0");
	masks[27]= OmnIpAddr("248.0.0.0");
	masks[28]= OmnIpAddr("240.0.0.0");
	masks[29]= OmnIpAddr("224.0.0.0");
	masks[30]= OmnIpAddr("192.0.0.0");
	masks[31]= OmnIpAddr("128.0.0.0");
	masks[32]= OmnIpAddr("0.0.0.0");


	for (int i=0; i<32; i++)
	{
		OmnTCBool(true, OmnIpAddr::isValidMask(masks[i])) << "mask: " << masks[i].toString() << endtc;
	}

	for (int i=0; i<mTries; i++)
	{
		u32 min = (u32)aos_next_int(0, 32);
		u32 max = (u32)aos_next_int(min, 32);
		// OmnIpAddr mask = aos_next_mask(min, max);
		OmnIpAddr mask = aos_next_mask();
		u32 minValue = (min)?(0xffffffff << (32-min)):0;
		u32 maxValue = (max)?(0xffffffff << (32-max)):0;

		bool found = false;
		for (int k=0; k<33; k++)
		{
			if (mask == masks[k]) 
			{
				found = true;
				break;
			}
		}
		OmnTCBool(true, found) << mask.toString() << endtc;
		OmnTCBool(true, OmnIpAddr::isValidMask(mask)) << "mask: " 
				<< mask.toString() << endtc;
		OmnTCBool(true, ntohl(mask.getIPv4()) >= minValue && ntohl(mask.getIPv4()) <= maxValue)
				<< "mask: " << mask.toString()
				<< " min: " << (int)min << ", max: " << (int)max  
				<< " minValue: " << (int)minValue << " maxValue: " << (int)maxValue
				<< " actualValue: " << (int)ntohl(mask.getIPv4()) << endtc;
	}

	return true;
}


bool
OmnRandomTester::testNextTimeStr(const u32 tries)
{
	for (u32 i=0; i<tries; i++)
	{
		cout << aos_next_time_str(eAosTimeStrFormat_HHMMSS).data() << endl;
	}

	return true;
}


bool
OmnRandomTester::testNextInvalidTimeStr(const u32 tries)
{
	for (u32 i=0; i<tries; i++)
	{
		cout << aos_next_invalid_time_str(1, 32).data() << endl;
	}

	return true;
}

