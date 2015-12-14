////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: route.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_route_h
#define Omn_aos_KernelSimu_route_h

#ifdef AOS_KERNEL_SIMULATE

#include "KernelSimu/dst.h"
#include "KernelSimu/flow.h"


#define RTO_ONLINK	0x01

#define RTO_CONN	0
/* RTO_CONN is not used (being alias for 0), but preserved not to break
 * some modules referring to it. */

#define RT_CONN_FLAGS(sk)   (RT_TOS(inet_sk(sk)->tos) | sk->sk_localroute)

struct inet_peer;
struct in_device;

struct rtable
{
	union
	{
		struct dst_entry	dst;
		struct rtable		*rt_next;
	} u;

	struct in_device		*idev;
	
	unsigned				rt_flags;
	unsigned				rt_type;

	__u32					rt_dst;	/* Path destination	*/
	__u32					rt_src;	/* Path source		*/
	int						rt_iif;
	__u32					rt_gateway;
	struct flowi			fl;
	__u32					rt_spec_dst; /* RFC1122 specific destination */
	struct inet_peer		*peer; /* long-living peer info */
};

struct ip_rt_acct
{
	__u32 	o_bytes;
	__u32 	o_packets;
	__u32 	i_bytes;
	__u32 	i_packets;
};

struct rt_cache_stat 
{
        unsigned int in_hit;
        unsigned int in_slow_tot;
        unsigned int in_slow_mc;
        unsigned int in_no_route;
        unsigned int in_brd;
        unsigned int in_martian_dst;
        unsigned int in_martian_src;
        unsigned int out_hit;
        unsigned int out_slow_tot;
        unsigned int out_slow_mc;
        unsigned int gc_total;
        unsigned int gc_ignored;
        unsigned int gc_goal_miss;
        unsigned int gc_dst_overflow;
        unsigned int in_hlist_search;
        unsigned int out_hlist_search;
};

extern void __ip_select_ident(struct iphdr *iph, struct dst_entry *dst, int more);
extern int ip_route_output_flow(struct rtable **rp, struct flowi *flp, struct sock *sk, int flags);
extern int __ip_route_output_key(struct rtable **rp, const struct flowi *flp);
extern struct rtable * aosGetRouteEntry(const unsigned char tos);
extern int aosInitRtTable();
#else
#include <net/route.h>
#endif

#endif

