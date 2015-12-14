////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: route.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/route.h"

#include "aos/aosKernelAlarm.h"
#include "aosUtil/Memory.h"
#include "KernelSimu/dev.h"
#include "KernelSimu/string.h"


const int sgRtTableSize = 256;
static struct rtable *rt_hash_table[sgRtTableSize];
static bool sgRtTableInitFlag = false;

void __ip_select_ident(struct iphdr *iph, struct dst_entry *dst, int more)
{
	//
	// Not sure what this function wants to do.
	//
	aosAlarmStr(eAosAlarm, "Function: __ip_select_ident() is called\n");
	
	/*
	struct rtable *rt = (struct rtable *) dst;

	if (rt) {
		if (rt->peer == NULL)
			rt_bind_peer(rt, 1);

		// If peer is attached to destination, it is never detached,
		//   so that we need not to grab a lock to dereference it.
		//
		if (rt->peer) {
			iph->id = htons(inet_getid(rt->peer, more));
			return;
		}
	} else
		printk(KERN_DEBUG "rt_bind_peer(0) @%p\n", NET_CALLER(iph));

	ip_select_fb_ident(iph);
	*/
}


// 
// This function selects a route based on the information in 'flp'. 
//
int ip_route_output_flow(struct rtable **rp, struct flowi *flp, struct sock *sk, int flags)
{
	return __ip_route_output_key(rp, flp);
}


int aosInitRtTable()
{
	sgRtTableInitFlag = true;
	int i;

	for (i=0; i<sgRtTableSize; i++)
	{
		rt_hash_table[i] = 0;
	}

	struct rtable *rt = (struct rtable*)aos_malloc(sizeof(struct rtable));
	memset(rt, 0, sizeof(struct rtable));
	rt->u.dst.dev = dev_get_by_name("eth0");
	rt_hash_table[0] = rt;

	rt = (struct rtable*)aos_malloc(sizeof(struct rtable));
	memset(rt, 0, sizeof(struct rtable));
	rt->u.dst.dev = dev_get_by_name("eth1");
	rt_hash_table[1] = rt;

	rt = (struct rtable*)aos_malloc(sizeof(struct rtable));
	memset(rt, 0, sizeof(struct rtable));
	rt->u.dst.dev = dev_get_by_name("eth2");
	rt_hash_table[2] = rt;

	return 0;
}


struct rtable * aosGetRouteEntry(const unsigned char tos)
{
	return rt_hash_table[tos];
}


// 
// This is the actual function to look up a route entry based on flowi.
// What it does is to hash on (saddr, daddr, oif, tos). It then looks 
// up the route table 'rt_hash_table[hash]', which is a chain of route
// entries with the same hash key. Route entries are selected based on
// the following:
//	dst, src, iif, oif, tos. 
//
// To simulate the function, we will maintain a route entry table indexed
// on tos. 
//
int __ip_route_output_key(struct rtable **rp, const struct flowi *flp)
{
	if (!sgRtTableInitFlag)
	{
		aosInitRtTable();
	}

	*rp = rt_hash_table[flp->fl4_tos];
	return 0;
}
