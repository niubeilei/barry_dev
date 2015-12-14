////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_sock_h
#define Omn_aos_KernelSimu_sock_h

#ifdef AOS_KERNEL_SIMULATE

#include <KernelSimu/list.h>
#include <KernelSimu/spinlock.h>
#include <KernelSimu/aosKernelMutex.h>
#include <KernelSimu/skbuff.h>

#include "KernelHooking/aosHooks.h"
#include "aos_core/aosSock.h"

#define SOCK_DEBUGGING
#ifdef SOCK_DEBUGGING
/* #define SOCK_DEBUG(sk, msg...) do { if ((sk) && ((sk)->sk_debug)) 
					printk(KERN_DEBUG msg); } while (0) */
#else
#define SOCK_DEBUG(sk, msg...) do { } while (0)
#endif

struct sock_iocb;
/*
typedef struct {
	spinlock_t		slock;
	struct sock_iocb	*owner;
	wait_queue_head_t	wq;
} socket_lock_t;
*/

/*
#define sock_lock_init(__sk) \
do {	spin_lock_init(&((__sk)->sk_lock.slock)); \
 	(__sk)->sk_lock.owner = NULL; \
 	init_waitqueue_head(&((__sk)->sk_lock.wq)); \
} while(0)
*/

struct sock;

/**
  *	struct sock_common - minimal network layer representation of sockets
  *	@skc_family - network address family
  *	@skc_state - Connection state
  *	@skc_reuse - %SO_REUSEADDR setting
  *	@skc_bound_dev_if - bound device index if != 0
  *	@skc_node - main hash linkage for various protocol lookup tables
  *	@skc_bind_node - bind hash linkage for various protocol lookup tables
  *	@skc_refcnt - reference count
  *
  *	This is the minimal network layer representation of sockets, the header
  *	for struct sock and struct tcp_tw_bucket.
  */
struct sock_common {
	unsigned short			skc_family;
	volatile unsigned char	skc_state;
	unsigned char			skc_reuse;
	int						skc_bound_dev_if;
	struct hlist_node		skc_node;
	struct hlist_node		skc_bind_node;
	aosKernelMutex			skc_refcntLock;
	int						skc_refcnt;
};

/**
  *	struct sock - network layer representation of sockets
  *	@__sk_common - shared layout with tcp_tw_bucket
  *	@sk_zapped - ax25 & ipx means !linked
  *	@sk_shutdown - mask of %SEND_SHUTDOWN and/or %RCV_SHUTDOWN
  *	@sk_use_write_queue - wheter to call sk->sk_write_space in sock_wfree
  *	@sk_userlocks - %SO_SNDBUF and %SO_RCVBUF settings
  *	@sk_lock -	synchronizer
  *	@sk_rcvbuf - size of receive buffer in bytes
  *	@sk_sleep - sock wait queue
  *	@sk_dst_cache - destination cache
  *	@sk_dst_lock - destination cache lock
  *	@sk_policy - flow policy
  *	@sk_rmem_alloc - receive queue bytes committed
  *	@sk_receive_queue - incoming packets
  *	@sk_wmem_alloc - transmit queue bytes committed
  *	@sk_write_queue - Packet sending queue
  *	@sk_omem_alloc - "o" is "option" or "other"
  *	@sk_wmem_queued - persistent queue size
  *	@sk_forward_alloc - space allocated forward
  *	@sk_allocation - allocation mode
  *	@sk_sndbuf - size of send buffer in bytes
  *	@sk_flags - %SO_LINGER (l_onoff), %SO_BROADCAST, %SO_KEEPALIVE, %SO_OOBINLINE settings
  *	@sk_no_check - %SO_NO_CHECK setting, wether or not checkup packets
  *	@sk_debug - %SO_DEBUG setting
  *	@sk_rcvtstamp - %SO_TIMESTAMP setting
  *	@sk_no_largesend - whether to sent large segments or not
  *	@sk_route_caps - route capabilities (e.g. %NETIF_F_TSO)
  *	@sk_lingertime - %SO_LINGER l_linger setting
  *	@sk_hashent - hash entry in several tables (e.g. tcp_ehash)
  *	@sk_backlog - always used with the per-socket spinlock held
  *	@sk_callback_lock - used with the callbacks in the end of this struct
  *	@sk_error_queue - rarely used
  *	@sk_prot - protocol handlers inside a network family
  *	@sk_err - last error
  *	@sk_err_soft - errors that don't cause failure but are the cause of a persistent failure not just 'timed out'
  *	@sk_ack_backlog - current listen backlog
  *	@sk_max_ack_backlog - listen backlog set in listen()
  *	@sk_priority - %SO_PRIORITY setting
  *	@sk_type - socket type (%SOCK_STREAM, etc)
  *	@sk_localroute - route locally only, %SO_DONTROUTE setting
  *	@sk_protocol - which protocol this socket belongs in this network family
  *	@sk_peercred - %SO_PEERCRED setting
  *	@sk_rcvlowat - %SO_RCVLOWAT setting
  *	@sk_rcvtimeo - %SO_RCVTIMEO setting
  *	@sk_sndtimeo - %SO_SNDTIMEO setting
  *	@sk_filter - socket filtering instructions
  *	@sk_protinfo - private area, net family specific, when not using slab
  *	@sk_slab - the slabcache this instance was allocated from
  *	@sk_timer - sock cleanup timer
  *	@sk_stamp - time stamp of last packet received
  *	@sk_socket - Identd and reporting IO signals
  *	@sk_user_data - RPC layer private data
  *	@sk_owner - module that owns this socket
  *	@sk_sndmsg_page - cached page for sendmsg
  *	@sk_sndmsg_off - cached offset for sendmsg
  *	@sk_send_head - front of stuff to transmit
  *	@sk_write_pending - a write to stream socket waits to start
  *	@sk_queue_shrunk - write queue has been shrunk recently
  *	@sk_state_change - callback to indicate change in the state of the sock
  *	@sk_data_ready - callback to indicate there is data to be processed
  *	@sk_write_space - callback to indicate there is bf sending space available
  *	@sk_error_report - callback to indicate errors (e.g. %MSG_ERRQUEUE)
  *	@sk_backlog_rcv - callback to process the backlog
  *	@sk_destruct - called at sock freeing time, i.e. when all refcnt == 0
 */
struct dns_entry;
struct sk_filter;

struct proto {
	void			(*close)(struct sock *sk, 
					long timeout);
};


static inline int sock_hold(struct sock *sk)
{
	return 0;
}


static inline int sock_put(struct sock *sk)
{
	return 0;
}

struct sock {
	/*
	 * Now struct tcp_tw_bucket also uses sock_common, so please just
	 * don't add nothing before this first member (__sk_common) --acme
	 */
	struct sock_common	__sk_common;
#define sk_family		__sk_common.skc_family
#define sk_state		__sk_common.skc_state
#define sk_reuse		__sk_common.skc_reuse
#define sk_bound_dev_if		__sk_common.skc_bound_dev_if
#define sk_node			__sk_common.skc_node
#define sk_bind_node		__sk_common.skc_bind_node
#define sk_refcnt		__sk_common.skc_refcnt
	unsigned char		sk_zapped;
	unsigned char		sk_shutdown;
	unsigned char		sk_use_write_queue;
	unsigned char		sk_userlocks;
	// socket_lock_t		sk_lock;
	int					sk_rcvbuf;

//	wait_queue_head_t		*sk_sleep;
	struct dst_entry		*sk_dst_cache;
	aosKernelMutex			sk_dst_lockLock;
	int						sk_dst_lock;
//	struct xfrm_policy		*sk_policy[2];
	aosKernelMutex			sk_rmem_allocLock;
	int						sk_rmem_alloc;
	struct sk_buff_head		sk_receive_queue;
	aosKernelMutex			sk_wmem_allocLock;
	int						sk_wmem_alloc;
	struct sk_buff_head		sk_write_queue;
	atomic_t				sk_omem_alloc;
	int						sk_wmem_queued;
	int						sk_forward_alloc;
	unsigned int			sk_allocation;
	int						sk_sndbuf;
	unsigned long 			sk_flags;
	char		 			sk_no_check;
	unsigned char			sk_debug;
	unsigned char			sk_rcvtstamp;
	unsigned char			sk_no_largesend;
	int						sk_route_caps;
	unsigned long	        sk_lingertime;
	int						sk_hashent;
	struct {
		struct sk_buff *head;
		struct sk_buff *tail;
	} sk_backlog;
//	rwlock_t				sk_callback_lock;
	struct sk_buff_head		sk_error_queue;
	struct proto			*sk_prot;
	int						sk_err,
							sk_err_soft;
	unsigned short			sk_ack_backlog;
	unsigned short			sk_max_ack_backlog;
	__u32					sk_priority;
	unsigned short			sk_type;
	unsigned char			sk_localroute;
	unsigned char			sk_protocol;
//	struct ucred			sk_peercred;
	int						sk_rcvlowat;
	long					sk_rcvtimeo;
	long					sk_sndtimeo;
	struct sk_filter      	*sk_filter;
	void					*sk_protinfo;
//	kmem_cache_t			*sk_slab;
//	struct timer_list		sk_timer;
//	struct timeval			sk_stamp;
	struct socket			*sk_socket;
	void					*sk_user_data;
//	struct module			*sk_owner;
//	struct page				*sk_sndmsg_page;
	__u32					sk_sndmsg_off;
	struct sk_buff			*sk_send_head;
	int						sk_write_pending;
	void					*sk_security;
	__u8					sk_queue_shrunk;
	void					(*sk_state_change)(struct sock *sk);
	void					(*sk_data_ready)(struct sock *sk, int bytes);
	void					(*sk_write_space)(struct sock *sk);
	void					(*sk_error_report)(struct sock *sk);
  	int						(*sk_backlog_rcv)(struct sock *sk,
						  		struct sk_buff *skb);  
	void                    (*sk_destruct)(struct sock *sk);

	// AosHook_005
	AosHook_005
	// End of AosHook_005
};

inline void lock_sock(struct sock *sk)
{
	// 
	// We don't care about the locking for now.
	//
}

inline void release_sock(struct sock *sk)
{
	// 
	// We don't care about the locking for now.
	//
}

extern int sock_setsockopt(struct socket *sock,
                    int level,
                    int optname,
                    char __user *optval,
                    int optlen);
extern void sock_init_data(struct socket *sock, struct sock *sk);

#else

#ifndef AOS_USERLAND
#include <net/sock.h>
#endif

#endif

#endif

