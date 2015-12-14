////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ip.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelPorting_ip_h
#define Omn_aos_KernelPorting_ip_h

#ifdef AOS_KERNEL_2_4_30
#define aosInetSock_saddr(x) ((struct sock*)x)->saddr
#define aosInetSock_sport(x) ((struct sock*)x)->sport
#define aosInetSock_rcv_addr(x) ((struct sock*)x)->rcv_saddr
#define aosInetSock_dport(x) ((struct sock*)x)->dport
#endif

#ifdef AOS_KERNEL_2_6_7

#define aosInetSock_saddr(x) ((struct inet_sock*)x)->inet.saddr
#define aosInetSock_sport(x) ((struct inet_sock*)x)->inet.sport
#define aosInetSock_rcv_addr(x) ((struct inet_sock*)x)->inet.rcv_saddr
#define aosInetSock_dport(x) ((struct inet_sock*)x)->inet.dport


#ifdef AOS_KERNEL_SIMULATE
struct inet_opt {
	/* Socket demultiplex comparisons on incoming packets. */
	__u32			daddr;					/* Foreign IPv4 addr */
	__u32			rcv_saddr;				/* Bound local IPv4 addr */
	__u16			dport;					/* Destination port */
	__u16			num;					/* Local port */
	__u32			saddr;					/* Sending source */
	int				uc_ttl;					/* Unicast TTL */
	int				tos;					/* TOS */
	unsigned	   	cmsg_flags;
	struct ip_options	*opt;
	__u16			sport;					/* Source port */
	unsigned char	hdrincl;				/* Include headers ? */
	__u8			mc_ttl;					/* Multicasting TTL */
	__u8			mc_loop;				/* Loopback */
	__u8			pmtudisc;
	__u16			id;						/* ID counter for DF pkts */
	unsigned		recverr : 1,
					freebind : 1;
	int				mc_index;				/* Multicast device index */
	__u32			mc_addr;
	// struct ip_mc_socklist	*mc_list;		/* Group array */
	struct page		*sndmsg_page;			/* Cached page for sendmsg */
	u32				sndmsg_off;					/* Cached offset for sendmsg */
	/*
	 * Following members are used to retain the infomation to build
	 * an ip header on each ip fragmentation while the socket is corked.
	 */
	struct {
		unsigned int		flags;
		unsigned int		fragsize;
		struct ip_options	*opt;
		struct rtable		*rt;
		int			length; /* Total length of all frames */
		u32			addr;
//		struct flowi		fl;
	} cork;
};

#define IPCORK_OPT	1	/* ip-options has been held in ipcork.opt */

struct ipv6_pinfo;

/* WARNING: don't change the layout of the members in inet_sock! */
struct inet_sock {
	struct sock	  sk;
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	struct ipv6_pinfo *pinet6;
#endif
	struct inet_opt   inet;
};

#endif // AOS_KERNEL_SIMULATE
#endif




#ifdef AOS_KERNEL_2_6_11

#define aosInetSock_saddr(x) ((struct inet_sock*)x)->saddr
#define aosInetSock_sport(x) ((struct inet_sock*)x)->sport
#define aosInetSock_rcv_addr(x) ((struct inet_sock*)x)->rcv_saddr
#define aosInetSock_dport(x) ((struct inet_sock*)x)->dport

struct inet_sock {
    struct sock     sk;
    __u32           daddr;              /* Foreign IPv4 addr */
    __u32           rcv_saddr;          /* Bound local IPv4 addr */
    __u16           dport;              /* Destination port */
    __u16           num;                /* Local port */
    __u32           saddr;              /* Sending source */
    int             uc_ttl;             /* Unicast TTL */
    int             tos;                /* TOS */
    unsigned        cmsg_flags;
    struct ip_options   *opt;
    __u16           sport;              /* Source port */
    unsigned char       hdrincl;        /* Include headers ? */
    __u8            mc_ttl;             /* Multicasting TTL */
    __u8            mc_loop;            /* Loopback */
    __u8            pmtudisc;
    __u16           id;                 /* ID counter for DF pkts */
    unsigned        recverr : 1,
                    freebind : 1;
    int             mc_index;           /* Multicast device index */
    __u32           mc_addr;
//  struct ip_mc_socklist   *mc_list;   /* Group array */
    /*
     * Following members are used to retain the infomation to build
     * an ip header on each ip fragmentation while the socket is corked.
     */
    struct {
        unsigned int        flags;
        unsigned int        fragsize;
        struct ip_options   *opt;
        struct rtable       *rt;
        int                 length;     /* Total length of all frames */
        u32                 addr;
//      struct flowi        fl;
    } cork;
};

#endif


#endif

