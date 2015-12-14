////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: skbuff.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelSimu_skbuff_h
#define aos_KernelSimu_skbuff_h

#ifndef __KERNEL__ 

#include <arpa/inet.h>

#include "KernelSimu/compiler.h"
#include "KernelSimu/gfp.h"
#include "KernelSimu/types.h"
#include "KernelSimu/page.h"
#include "KernelSimu/atomic.h"
#include "KernelSimu/cache.h"

#include "aos/aosKernelAlarm.h"
#include "Porting/Mutex.h"

#define HAVE_ALLOC_SKB		/* For the drivers to know */
#define HAVE_ALIGNABLE_SKB	/* Ditto 8)		   */
#define SLAB_SKB 		/* Slabified skbuffs 	   */

#define CHECKSUM_NONE 0
#define CHECKSUM_HW 1
#define CHECKSUM_UNNECESSARY 2

#define SKB_DATA_ALIGN(X)	(((X) + (SMP_CACHE_BYTES - 1)) & \
		~(SMP_CACHE_BYTES - 1))
#define SKB_MAX_ORDER(X, ORDER)	(((PAGE_SIZE << (ORDER)) - (X) - \
			sizeof(struct skb_shared_info)) & \
		~(SMP_CACHE_BYTES - 1))
#define SKB_MAX_HEAD(X)		(SKB_MAX_ORDER((X), 0))
#define SKB_MAX_ALLOC		(SKB_MAX_ORDER(0, 2))

struct net_device;
struct tcphdr;
struct udphdr;
struct icmphdr;
struct iphdr;
struct ipv6hdr;
struct arphdr;
struct dst_entry;
struct sec_path;
struct sock;
struct nf_conntrack;
struct nf_bridge_info;
struct page;

#define MAX_SKB_FRAGS (65536/PAGE_SIZE + 2)

#include <sys/time.h>
#include <time.h>

struct sk_buff {
	struct sk_buff		*next;
	struct sk_buff		*prev;

	struct sk_buff_head	*list;
	struct sock			*sk;
	//struct timeval		stamp;
	struct net_device	*dev;
	struct net_device	*input_dev;
	struct net_device	*real_dev;

	char freeflag;

	// Layer 4 header
	union {
		struct tcphdr	*th;
		struct udphdr	*uh;
		struct icmphdr	*icmph;
		struct igmphdr	*igmph;
		struct iphdr	*ipiph;
		struct ipv6hdr	*ipv6h;
		unsigned char	*raw;
	} h;

	// Layer 3 header
	union {
		struct iphdr	*iph;
		struct ipv6hdr	*ipv6h;
		struct arphdr	*arph;
		unsigned char	*raw;
	} nh;

	// Layer 2 header
	union {
		unsigned char 	*raw;
	} mac;

	struct  dst_entry	*dst;
	struct	sec_path	*sp;

	/*
	 * This is the control buffer. It is free to use for every
	 * layer. Please put your private variables there. If you
	 * want to keep them across layers you have to do a skb_clone()
	 * first. This is owned by whoever has the skb queued ATM.
	 */
	char				cb[40];

	unsigned int		len,
						data_len,
						mac_len,
						csum;
	unsigned char		local_df,
						cloned,
						pkt_type,
						ip_summed;
	__u32				priority;
	unsigned short		protocol,
						security;

	void			(*destructor)(struct sk_buff *skb);
#ifdef CONFIG_NETFILTER
	unsigned long	nfmark;
	__u32				nfcache;
	__u32				nfctinfo;
	struct nf_conntrack	*nfct;
#ifdef CONFIG_NETFILTER_DEBUG
	unsigned int		nf_debug;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	struct nf_bridge_info	*nf_bridge;
#endif
#endif /* CONFIG_NETFILTER */
#if defined(CONFIG_HIPPI)
	union {
		__u32		ifield;
	} private;
#endif
#ifdef CONFIG_NET_SCHED
	__u32			tc_index;        /* traffic control index */
#ifdef CONFIG_NET_CLS_ACT
	__u32           tc_verd;               /* traffic control verdict */
	__u32           tc_classid;            /* traffic control classid */
#endif

#endif

#ifdef CONFIG_AOS_QOS
	struct _qos_traffic_channel *aos_qos_ch; /* aos qos traffic control channel */
#endif

	unsigned short     vid;

	/* These elements must be at the end, see alloc_skb() for details.  */
	unsigned int		truesize;
	// OmnMutexType		usersLock;
	int					users;
	unsigned char		*head,
						*data,
						*tail,
						*end;
};

typedef struct skb_frag_struct skb_frag_t;

struct skb_frag_struct 
{
	struct page 	*page;
	__u16 			page_offset;
	__u16 			size;
};


struct skb_shared_info {
	OmnMutexType		datarefLock;
	int					dataref;
	unsigned int		nr_frags;
	unsigned short		tso_size;
	unsigned short		tso_segs;
	struct sk_buff		*frag_list;
	skb_frag_t			frags[MAX_SKB_FRAGS];
};

typedef int gfp_t;

extern void __kfree_skb(struct sk_buff *skb);
extern void skb_under_panic(struct sk_buff *skb, int sz, void *here);
extern struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask);
extern struct sk_buff *skb_copy_expand(const struct sk_buff *skb,
		int newheadroom, int newtailroom,
		gfp_t priority);
extern void skb_split(struct sk_buff *skb, struct sk_buff *skb1, const u32 len);

#define skb_shinfo(SKB)		((struct skb_shared_info *)((SKB)->end))

inline void kfree_skb(struct sk_buff *skb)
{
	//OmnMutexLock(skb->usersLock);
	if (skb->users == 1)
	{
		//
		// Is this right?
		//
		aosAlarm(eAosAlarm);
		//OmnMutexUnlock(skb->usersLock);
		return;
	}

	(skb->users)--;
	if (skb->users > 0)
	{
		// 
		// Do not delete it yet
		//
		//OmnMutexUnlock(skb->usersLock);
		return;
	}

	//OmnMutexUnlock(skb->usersLock);
	__kfree_skb(skb);
}


static inline unsigned char *skb_push(struct sk_buff *skb, unsigned int len)
{
	skb->data -= len;
	skb->len  += len;
	if (unlikely(skb->data<skb->head))
	{
		skb_under_panic(skb, len, 0);
	}
	return skb->data;
}


struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	__u32			qlen;
	OmnMutexType	lock;
};

extern struct sk_buff *skb_dequeue(struct sk_buff_head *list);

static inline struct sk_buff *__skb_dequeue(struct sk_buff_head *list)
{
	struct sk_buff *next, *prev, *result;

	prev = (struct sk_buff *) list;
	next = prev->next;
	result = NULL;
	if (next != prev) {
		result	     = next;
		next	     = next->next;
		list->qlen--;
		next->prev   = prev;
		prev->next   = next;
		result->next = result->prev = NULL;
		result->list = NULL;
	}
	return result;
}


static inline unsigned char *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	skb->len -= len;
	if (skb->len < skb->data_len)
	{
		aosAlarm(eAosAlarm);
	}
	return skb->data += len;
}

extern struct sk_buff *alloc_skb(unsigned int size, int gfp_mask);

// 
// It moves the data portion forward. Normally this is called before
// data portion is actually used. Otherwise, it can cause problem.
//
static inline void skb_reserve(struct sk_buff *skb, unsigned int len)
{
	skb->data += len;
	skb->tail += len;
}

static inline int skb_is_nonlinear(const struct sk_buff *skb)
{
	return skb->data_len;
}

static inline unsigned int skb_headlen(const struct sk_buff *skb)
{
	return skb->len - skb->data_len;
}

#define SKB_PAGE_ASSERT(skb) 	//BUG_ON(skb_shinfo(skb)->nr_frags)
#define SKB_FRAG_ASSERT(skb) 	//BUG_ON(skb_shinfo(skb)->frag_list)
#define SKB_LINEAR_ASSERT(skb)  //BUG_ON(skb_is_nonlinear(skb))

/*
 *	Add data to an sk_buff
 **/
static inline unsigned char *__skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = skb->tail;
	SKB_LINEAR_ASSERT(skb);
	skb->tail += len;
	skb->len  += len;
	return tmp;
}

// 
// It moves 'tail' for 'len' number of bytes, making room for the data. 
// If one wants a skb to expand its data portion, call this function.
// It is important to make sure not to run out of the bound.
//
static inline unsigned char *skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = skb->tail;
	skb->tail += len;
	skb->len  += len;
	if (skb->tail>skb->end)
	{
		aosAlarm(eAosAlarm);
		// skb_over_panic(skb, len, current_text_addr());
	}
	return tmp;
}

extern void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);
static inline void __skb_queue_tail(struct sk_buff_head *list,
		struct sk_buff *newsk)
{
	struct sk_buff *prev, *next;

	newsk->list = list;
	list->qlen++;
	next = (struct sk_buff *)list;
	prev = next->prev;
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
}

static inline struct sk_buff *skb_peek(struct sk_buff_head *list_)
{
	struct sk_buff *list = ((struct sk_buff *)list_)->next;
	if (list == (struct sk_buff *)list_)
		list = NULL;
	return list;
}

static inline __u32 skb_queue_len(const struct sk_buff_head *list_)
{
	return list_->qlen;
}

extern unsigned int skb_checksum(const struct sk_buff *skb, int offset,
		int len, unsigned int csum);

#define skb_queue_walk(queue, skb) \
	for (skb = (queue)->next;					\
			(skb != (struct sk_buff *)(queue));	\
			skb = skb->next)

static inline void skb_queue_head_init(struct sk_buff_head *list)
{
	OmnInitMutex(list->lock);
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}

extern void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk);
static inline void __skb_queue_head(struct sk_buff_head *list,
		struct sk_buff *newsk)
{
	struct sk_buff *prev, *next;

	newsk->list = list;
	list->qlen++;
	prev = (struct sk_buff *)list;
	next = prev->next;
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
}


static inline int skb_queue_empty(const struct sk_buff_head *list)
{
	return list->next == (struct sk_buff *)list;
}

extern void skb_queue_purge(struct sk_buff_head *list);

static inline int skb_headroom(const struct sk_buff *skb)
{
	return skb->data - skb->head;
}

static inline int skb_tailroom(const struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}
/**
 *	skb_shared - is the buffer shared
 *	@skb: buffer to check
 *
 *	Returns true if more than one person has a reference to this
 *	buffer.
 */
static inline int skb_shared(const struct sk_buff *skb)
{
		return atomic_read(&skb->users) != 1;
}
#else
#include <linux/skbuff.h>
#endif

#endif

