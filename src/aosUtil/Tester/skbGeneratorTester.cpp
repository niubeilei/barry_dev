////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: skbGeneratorTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtilTester/skbGeneratorTester.h"

#include "aos/aosBridge.h"
#include "aosUtil/aosIpOutput.h"
#include "aos/aosCorePcp.h"

#include <KernelSimu/route.h>

#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"



bool AosSkbGeneratorTester::start()
{
	// 
	// Test default constructor
	//
	// aosBridge_init();
	testSkbGeneration();
	return true;
}


bool AosSkbGeneratorTester::testSkbGeneration()
{
	/*
	OmnString rslt;
	const int size = 999+1;
	char data[size];

	 * Commented out by Chen Ding, 08/29/2005
	 *
	memset(data, 'c', size);
	OmnBeginTest << "Test SKB Generation";
	mTcNameRoot = "SkbGeneration";

	int priority = 0;
	struct rtable *rt = 0;
	struct ip_options *opt = 0;
	unsigned int saddr = 0x01020304;
	unsigned int daddr = 0x04030201;
	unsigned int sport = 5000;
	unsigned int dport = 6000;
	unsigned int flags = 0;
	unsigned char doChecksum = 0;
	unsigned char identity = 10;
	unsigned char protocol = 17;
	unsigned char tos = 5;

	// 
	// Get a route entry.
	//
	struct sk_buff *skb = 0;
	rt = aosGetRouteEntry(0);
	int ret = aosCreateSkb(&skb, data, size, priority, rt, opt, saddr, daddr, 
		sport, dport, flags, doChecksum, identity, protocol, tos);

	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(skb != 0)) << endtc;

	int rc = aosNetInputPcp_proc(skb);
	OmnTC(OmnExpected<int>(eAosRc_PktConsumed), OmnActual<int>(rc)) << endtc;

	*/
	return true;
}



