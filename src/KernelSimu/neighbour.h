////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: neighbour.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_neighbour_h
#define Omn_aos_KernelSimu_neighbour_h

#ifdef AOS_KERNEL_SIMULATE

#include "KernelSimu/aosKernelMutex.h"
#include "KernelSimu/rcupdate.h"
#include "KernelSimu/jiffies.h"

#define NTF_PROXY	0x08	/* == ATF_PUBL */
#define NTF_ROUTER	0x80

/*
 *	Neighbor Cache Entry States.
 */

#define NUD_INCOMPLETE		0x01
#define NUD_REACHABLE		0x02
#define NUD_STALE			0x04
#define NUD_DELAY			0x08
#define NUD_PROBE			0x10
#define NUD_FAILED			0x20

/* Dummy states */
#define NUD_NOARP			0x40
#define NUD_PERMANENT		0x80
#define NUD_NONE			0x00


#define NUD_IN_TIMER	(NUD_INCOMPLETE|NUD_REACHABLE|NUD_DELAY|NUD_PROBE)
#define NUD_VALID	(NUD_PERMANENT|NUD_NOARP|NUD_REACHABLE|NUD_PROBE|NUD_STALE|NUD_DELAY)
#define NUD_CONNECTED	(NUD_PERMANENT|NUD_NOARP|NUD_REACHABLE)

struct neighbour;
struct neigh_table;

struct neigh_parms
{
	struct neigh_parms 		*next;
	int	(*neigh_setup)(struct neighbour *);
	struct neigh_table 		*tbl;
	int						entries;
	void					*priv;

	void					*sysctl_table;

	int 					dead;

	OmnMutexType			refcntLock;
	int						refcnt;
	struct rcu_head 		rcu_head;

	int						base_reachable_time;
	int						retrans_time;
	int						gc_staletime;
	int						reachable_time;
	int						delay_probe_time;

	int						queue_len;
	int						ucast_probes;
	int						app_probes;
	int						mcast_probes;
	int						anycast_delay;
	int						proxy_delay;
	int						proxy_qlen;
	int						locktime;
};

struct neigh_statistics
{
	unsigned long allocs;		/* number of allocated neighs */
	unsigned long destroys;		/* number of destroyed neighs */
	unsigned long hash_grows;	/* number of hash resizes */

	unsigned long res_failed;	/* nomber of failed resolutions */

	unsigned long lookups;		/* number of lookups */
	unsigned long hits;		/* number of hits (among lookups) */

	unsigned long rcv_probes_mcast;	/* number of received mcast ipv6 */
	unsigned long rcv_probes_ucast; /* number of received ucast ipv6 */

	unsigned long periodic_gc_runs;	/* number of periodic GC runs */
	unsigned long forced_gc_runs;	/* number of forced GC runs */
};


struct neighbour
{
	struct neighbour		*next;
	struct neigh_table		*tbl;
	struct neigh_parms		*parms;
	struct net_device		*dev;
	unsigned long			used;
	unsigned long			confirmed;
	unsigned long			updated;
	__u8					flags;
	__u8					nud_state;
	__u8					type;
	__u8					dead;
	OmnMutexType			probesLock;
	int						probes;
	OmnMutexType			lock;
	unsigned char		ha[(MAX_ADDR_LEN+sizeof(unsigned long)-1)&~(sizeof(unsigned long)-1)];
	struct hh_cache			*hh;
	OmnMutexType			refcntLock;
	int						refcnt;
	int			(*output)(struct sk_buff *skb);
	struct sk_buff_head		arp_queue;
	struct timer_list		timer;
	struct neigh_ops	*	ops;
	u8						primary_key;
};

struct neigh_ops
{
	int			family;
	void		(*destructor)(struct neighbour *);
	void		(*solicit)(struct neighbour *, struct sk_buff*);
	void		(*error_report)(struct neighbour *, struct sk_buff*);
	int			(*output)(struct sk_buff*);
	int			(*connected_output)(struct sk_buff*);
	int			(*hh_output)(struct sk_buff*);
	int			(*queue_xmit)(struct sk_buff*);
};


static inline struct neighbour *
__neigh_lookup(struct neigh_table *tbl, const void *pkey, struct net_device *dev, int creat)
{
	/*
	struct neighbour *n = neigh_lookup(tbl, pkey, dev);

	if (n || !creat)
		return n;

	n = neigh_create(tbl, pkey, dev);
	return IS_ERR(n) ? NULL : n;
	*/

	return 0;
}


static inline int neigh_event_send(struct neighbour *neigh, struct sk_buff *skb)
{
	neigh->used = (unsigned long)jiffies;
	if (!(neigh->nud_state&(NUD_CONNECTED|NUD_DELAY|NUD_PROBE)))
	{
		printk("KernelSimu: __neigh_event_send\n");
		// return __neigh_event_send(neigh, skb);
	}
	return 0;
}


static inline void neigh_release(struct neighbour *neigh)
{
	aosKernelLock(neigh->refcntLock);
	(neigh->refcnt)--;
	if (neigh->refcnt <= 0)
	{
		printk("KernelSimu: to delete neighbour\n");
		// neigh_destroy(neigh);
	}
}


#else
#include <net/neighbour.h>
#endif

#endif

