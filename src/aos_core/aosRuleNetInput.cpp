////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosRuleNetInput.cpp
// Description:
//	NetInputRule is a built-in rule that is installed in NetInputPcp.
//	This rule assumes a dynamic array that is accessed by:
//		Sender IP
//		Sender Port
//		Receiver IP
//		Receiver Port
//		Protocol
//	When it is called, it looks up the array. If a match is found, 
//  it calls its action.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*
#include "aos/aosNetInputRule.h"

#include <KernelSimu/netdevice.h>
#include <KernelSimu/skbuff.h>
#include <KernelSimu/if_ether.h>
#include "aos/KernelEnum.h"
#include "aos/aosRule.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosBridge.h"
#include "aos/aosCorePcp.h"


// 
// This function is called when an skb is received from the network.
// If the packet is processed, it returns 'aosPacketConsumed'. Otherwise, 
// it returns 0, meaning the packet should continue its normal processing.
//
int aosNetInputRule_proc(struct skb_buff *skb)
{
	struct aosNetInputRslt *rslt;
	// 
	// Look up the array.
	//
	if (!rslt)
	{
		// 
		// Didn't find any.
		//
		return 0;
	}

	// 
	// If the result contains a rule, use that rule.
	//
	if (rslt->mRule)
	{
		if (rslt->mRule->mProc)
		{
			return rslt->mRule->mProc(skb);
		}

		// 
		// Should never happen
		//
		aosAlarm(eAosAlarmRuleProcNull);
		return 0;
	}

	// 
	// We may allow some built-in processing here. 
	// For now, we simply return.
	//
	return 0;
}
*/

