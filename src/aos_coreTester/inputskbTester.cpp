////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: inputskbTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_coreTester/inputSkbTester.h"

#include <KernelSimu/skbuff.h>

#include "Debug/Debug.h"
#include "aos/aosKernelError.h"
#include "aos_core/aosDevTest.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"



bool OmnInputSkbTester::start()
{
	// 
	// Test default constructor
	//
	testInputSkb();
	return true;
}


bool OmnInputSkbTester::testInputSkb()
{
	struct sk_buff *skb = alloc_skb(1010, 1);
	if (!skb)
	{
		aosAlarm(eAosAlarmMemErr);
		return false;
	}

	netif_receive_skb(skb);
	return true;
}


