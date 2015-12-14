////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosRuleHeadSwapper.cpp
// Description:
//	This rule swaps the header of a packet and forwards the packet 
//  through a specified device. It can be useful for VoIP or other
//  types of streaming based applications.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

// #include "aos/aosRuleHeadSwapper.h"

#include <KernelSimu/netdevice.h>
#include <KernelSimu/skbuff.h>
#include <KernelSimu/if_ether.h>
#include "aos/KernelEnum.h"
#include "aos_core/aosRule.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosBridge.h"
#include "aos/aosCorePcp.h"


/*
// 
// It does the following:
//	1. Change the sender and receiver IP addresses and ports
//  2. Recalculate the IP header checksum
//  3. Forward the packet through the specified device.
//
// It assumes:
//	1. the caller has verified it is an IP packet
//
int aosRuleHeadSwapper_proc(struct skb_buff *skb)
{
	struct iphdr *iph = aosGetIphdr(skb);
	
	if (!iph)
	{
		// 
		// This should never happen. 
		// 
		aosAlarmRated(eAosAlarmFatal, 5, 0, 0, 0, 0);
		return 0;
	}

	switch (iph->protocol)
	{
	case UDP:
		 udphdr = aosGetUdphdr(skb);
		 if (!udphdr)
		 {
			// should never happen. 
			return 0;
		 }
		 break;

	case TCP:
		 tcphdr = aosGetTcphdr(skb);
		 if (!tcphdr)
		 {
			return 0;	
		 }
		 break;

	default:
		 return 0;
	}

	...	

	return 0;
}
*/


