////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: dst.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_net_dst_h
#define Omn_aos_KernelSimu_net_dst_h

#ifdef AOS_KERNEL_SIMULATE

#include "aos/aosKernelAlarm.h"
#include <KernelSimu/types.h>
#include <KernelSimu/aosKernelMutex.h>
#include <KernelSimu/rtnetlink.h>
#include <KernelSimu/rcupdate.h>
#include <KernelSimu/netdevice.h>
#include <KernelSimu/skbuff.h>

#define RT_CACHE_DEBUG		0

#define DST_GC_MIN	(HZ/10)
#define DST_GC_INC	(HZ/2)
#define DST_GC_MAX	(120*HZ)


struct sk_buff;
struct net_device;
struct neighbour;
struct hh_cache;
struct xfrm_state;
struct dst_ops;
struct rcu_head;

struct dst_entry
{
	struct dst_entry        *next;
	OmnMutexType			__refcntLock;
	int						__refcnt;	/* client references	*/
	int						__use;
	struct dst_entry		*child;
	struct net_device       *dev;
	int						obsolete;
	int						flags;
#define DST_HOST			1
#define DST_NOXFRM			2
#define DST_NOPOLICY		4
#define DST_NOHASH			8
	unsigned long			lastuse;
	unsigned long			expires;

	unsigned short			header_len;	/* more space at head required */
	unsigned short			trailer_len;	/* space to reserve at tail */

	u32						metrics[RTAX_MAX];
	struct dst_entry		*path;

	unsigned long			rate_last;	/* rate limiting for ICMP */
	unsigned long			rate_tokens;

	int						error;

	struct neighbour		*neighbour;
	struct hh_cache			*hh;
	struct xfrm_state		*xfrm;

	int			(*input)(struct sk_buff*);
	int			(*output)(struct sk_buff*);

#ifdef CONFIG_NET_CLS_ROUTE
	u32						tclassid;
#endif

	struct  dst_ops	        *ops;
	struct rcu_head			rcu_head;
		
	char					info[1];
};


static inline
void dst_release(struct dst_entry * dst)
{
	if (dst) 
	{
		aosKernelLock(dst->__refcntLock);
		if (dst->__refcnt < 1)
		{
			aosAlarm(eAosAlarm);
		}
		(dst->__refcnt)--;
		aosKernelUnlock(dst->__refcntLock);
	}
}


static inline
struct dst_entry * dst_clone(struct dst_entry * dst)
{
	if (dst)
	{
		aosKernelLock(dst->__refcntLock);
		(dst->__refcnt)++;
		aosKernelUnlock(dst->__refcntLock);
	}
		
	return dst;
}


static inline int dst_output(struct sk_buff *skb)
{
	int err;

	for (;;) 
	{
		err = skb->dst->output(skb);

		if (err == 0)
		{
			return err;
		}

		if (err != NET_XMIT_BYPASS)
		{
			return err;
		}
	}
}


static inline u32
dst_pmtu(const struct dst_entry *dst)
{
	return 1500;
	// u32 mtu = dst_path_metric(dst, RTAX_MTU);
	// barrier();
	// return mtu;
}


#else
#include <net/dst.h>
#endif

#endif

