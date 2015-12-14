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
#if 0
#include "Random/Tester/RandomUtilTester.h"

#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/MacAddr.h"
#include "Util/TimeStr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

OmnRandomUtilTester::OmnRandomUtilTester()
{
	mName = "OmnRandomUtilTester";
	mTries = 100000;
	mTestNextIntTries = 100;
	mTestNextIncorrectIpTries = 100;
	mTestNextIpTries = 100000;
}


bool OmnRandomUtilTester::start()
{
	u32 tries = 1000000 * 100;
	testMac(tries);
	testNextInt(tries);
	testNextIncorrectIp(tries);
	testNextIP(tries);
	testNextMask(tries);
	testNextTimeStr(tries);
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
OmnRandomUtilTester::testNextInt(const u32 tries)
{
	u32 count = 10000;
	for (u32 i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Trying next int: " << i << endl;
			count += 10000;
		}

		int min = rand();
		int max = rand();
		if (rand() & 0xff == 1) min = -min;
		if (rand() & 0xff == 1) max = -max;
		if (min > max)
		{
			OmnTCTrue(OmnActual<bool>(aos_next_int(min, max) == min)) << endtc;
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
OmnRandomUtilTester::testNextIncorrectIp(const u32 tries)
{
	OmnString str;
	u32 count = 10000;
	for (u32 i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Trying next incorrect ip: " << i << endl;
			count += 10000;
		}

		str = aos_next_incorrect_ip();
		OmnTC(OmnExpected<bool>(false), OmnActual<bool>(OmnIpAddr::isValidAddr(str))) << endtc;
	}

	return true;
}


bool
OmnRandomUtilTester::testNextIP(const u32 tries)
{
	OmnIpAddr a;
	OmnString str;
	u32 count = 10000;
	for (u32 i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Trying next IP: " << i << endl;
			count += 10000;
		}

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
OmnRandomUtilTester::testNextMask(const u32 tries)
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
		OmnTCBool(true, OmnIpAddr::isValidMask(masks[i])) 
			<< "mask: " << masks[i].toString() << endtc;
	}

	u32 count = 10000;
	for (u32 i=0; i<tries; i++)
	{
		if (i>=count)
		{
			cout << "Trying next mask: " << i << endl;
			count += 10000;
		}

		OmnIpAddr mask = aos_next_mask();

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
	}

	return true;
}


bool
OmnRandomUtilTester::testNextTimeStr(const u32 tries)
{
	u32 count = 10000;
	for (u32 i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Trying next time string: " << i << endl;
			count += 10000;
		}

		OmnString str = aos_next_time_str();
		OmnTCTrue(aos_is_valid_timestr(str)) << str << endtc;
	}

	return true;
}


bool
OmnRandomUtilTester::testNextInvalidTimeStr(const u32 tries)
{
	u32 count = 10000;
	for (u32 i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Trying next invalid timestr: " << i << endl;
			count += 10000;
		}

		OmnString str = aos_next_invalid_time_str(1, 9);
		OmnTCTrue(!aos_is_valid_timestr(str)) << str << endtc;
	}

	return true;
}


bool
OmnRandomUtilTester::testMac(const u32 tries)
{
	u32 i;
	u32 count = 10000;
	for (i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Tried correct: " << i << endl;
			count += 10000;
		}

		OmnMacAddr mac = aos_next_mac();
		// cout << mac.toString() << endl;
		OmnTCTrue(mac.isValid()) << mac.toString() << endtc;
	}

	count = 10000;
	for (i=0; i<tries; i++)
	{
		if (i >= count)
		{
			cout << "Tried incorrect: " << i << endl;
			count += 10000;
		}

		OmnString mac = aos_next_incorrect_mac();
		// cout << mac << endl;
		OmnMacAddr mmm(mac);
		OmnTCTrue(!mmm.isValid()) << mac << endtc;
	}
	return true;
}
#endif
