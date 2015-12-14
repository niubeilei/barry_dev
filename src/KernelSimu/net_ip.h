////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: net_ip.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_net_ip_h
#define Omn_aos_KernelSimu_net_ip_h

#ifdef AOS_KERNEL_SIMULATE

#include <KernelSimu/snmp.h>

struct sock;

struct ipcm_cookie
{
	u32			addr;
	int			oif;
	struct ip_options	*opt;
};

#define IPCB(skb) ((struct inet_skb_parm*)((skb)->cb))

struct ip_ra_chain
{
	struct ip_ra_chain	*next;
	struct sock		*sk;
	void			(*destructor)(struct sock *);
};

extern struct ip_ra_chain *ip_ra_chain;

/* IP flags. */
#define IP_CE		0x8000		/* Flag: "Congestion"		*/
#define IP_DF		0x4000		/* Flag: "Don't Fragment"	*/
#define IP_MF		0x2000		/* Flag: "More Fragments"	*/
#define IP_OFFSET	0x1FFF		/* "Fragment Offset" part	*/

#define IP_FRAG_TIME	(30 * HZ)		/* fragment lifetime	*/

extern void		ip_mc_dropsocket(struct sock *);
extern void		ip_mc_dropdevice(struct net_device *dev);
extern int		igmp_mc_proc_init(void);

/*
 *	Functions provided by ip.c
 */

extern int		ip_build_and_send_pkt(struct sk_buff *skb, struct sock *sk,
					      u32 saddr, u32 daddr,
					      struct ip_options *opt);
extern int		ip_rcv(struct sk_buff *skb, struct net_device *dev,
			       struct packet_type *pt);
extern int		ip_local_deliver(struct sk_buff *skb);
extern int		ip_mr_input(struct sk_buff *skb);
extern int		ip_output(struct sk_buff *skb);
extern int		ip_mc_output(struct sk_buff *skb);
extern int		ip_fragment(struct sk_buff *skb, int (*out)(struct sk_buff*));
extern int		ip_do_nat(struct sk_buff *skb);
extern void		ip_send_check(struct iphdr *ip);
extern int		ip_queue_xmit(struct sk_buff *skb, int ipfragok);
extern void		ip_init(void);
extern int		ip_append_data(struct sock *sk,
				       int getfrag(void *from, char *to, int offset, int len,
						   int odd, struct sk_buff *skb),
				void *from, int len, int protolen,
				struct ipcm_cookie *ipc,
				struct rtable *rt,
				unsigned int flags);
extern int		ip_generic_getfrag(void *from, char *to, int offset, int len, int odd, struct sk_buff *skb);
extern ssize_t	ip_append_page(struct sock *sk, struct page *page,
				int offset, size_t size, int flags);
extern int		ip_push_pending_frames(struct sock *sk);
extern void		ip_flush_pending_frames(struct sock *sk);

/* datagram.c */
extern int		ip4_datagram_connect(struct sock *sk, 
					     struct sockaddr *uaddr, int addr_len);

/*
 *	Map a multicast IP onto multicast MAC for type Token Ring.
 *      This conforms to RFC1469 Option 2 Multicasting i.e.
 *      using a functional address to transmit / receive 
 *      multicast packets.
 */

static inline void ip_tr_mc_map(u32 addr, char *buf)
{
	buf[0]=(char)0xC0;
	buf[1]=(char)0x00;
	buf[2]=(char)0x00;
	buf[3]=(char)0x04;
	buf[4]=(char)0x00;
	buf[5]=(char)0x00;
}


void ip_send_reply(struct sock *sk, struct sk_buff *skb, struct ip_reply_arg *arg,
		   unsigned int len); 

extern int ip_finish_output(struct sk_buff *skb);

struct ipv4_config
{
	int	log_martians;
	int	autoconfig;
	int	no_pmtu_disc;
};

extern struct ipv4_config ipv4_config;
#define IP_INC_STATS(field)		
#define IP_INC_STATS_BH(field)		SNMP_INC_STATS_BH(ip_statistics, field)
#define IP_INC_STATS_USER(field) 	SNMP_INC_STATS_USER(ip_statistics, field)
#define NET_INC_STATS(field)		SNMP_INC_STATS(net_statistics, field)
#define NET_INC_STATS_BH(field)		SNMP_INC_STATS_BH(net_statistics, field)
#define NET_INC_STATS_USER(field) 	SNMP_INC_STATS_USER(net_statistics, field)
#define NET_ADD_STATS_BH(field, adnd)	SNMP_ADD_STATS_BH(net_statistics, field, adnd)
#define NET_ADD_STATS_USER(field, adnd)	SNMP_ADD_STATS_USER(net_statistics, field, adnd)

extern int sysctl_local_port_range[2];
extern int sysctl_ip_default_ttl;

#ifdef CONFIG_INET
/* The function in 2.2 was invalid, producing wrong result for
 * check=0xFEFF. It was noticed by Arthur Skawina _year_ ago. --ANK(000625) */
static inline
int ip_decrease_ttl(struct iphdr *iph)
{
	u32 check = iph->check;
	check += htons(0x0100);
	iph->check = check + (check>=0xFFFF);
	return --iph->ttl;
}

static inline
int ip_dont_fragment(struct sock *sk, struct dst_entry *dst)
{
	return (inet_sk(sk)->pmtudisc == IP_PMTUDISC_DO ||
		(inet_sk(sk)->pmtudisc == IP_PMTUDISC_WANT &&
		 !(dst_metric(dst, RTAX_LOCK)&(1<<RTAX_MTU))));
}

extern void __ip_select_ident(struct iphdr *iph, struct dst_entry *dst, int more);

static inline void ip_select_ident(struct iphdr *iph, struct dst_entry *dst, struct sock *sk)
{
	if (iph->frag_off & htons(IP_DF)) {
		/* This is only to work around buggy Windows95/2000
		 * VJ compression implementations.  If the ID field
		 * does not change, they drop every other packet in
		 * a TCP stream using header compression.
		 */
		iph->id = (sk && inet_sk(sk)->daddr) ?
					htons(inet_sk(sk)->id++) : 0;
	} else
		__ip_select_ident(iph, dst, 0);
}

static inline void ip_select_ident_more(struct iphdr *iph, struct dst_entry *dst, struct sock *sk, int more)
{
	if (iph->frag_off & htons(IP_DF)) {
		if (sk && inet_sk(sk)->daddr) {
			iph->id = htons(inet_sk(sk)->id);
			inet_sk(sk)->id += 1 + more;
		} else
			iph->id = 0;
	} else
		__ip_select_ident(iph, dst, more);
}

/*
 *	Map a multicast IP onto multicast MAC for type ethernet.
 */

static inline void ip_eth_mc_map(u32 addr, char *buf)
{
	addr=ntohl(addr);
	buf[0]=0x01;
	buf[1]=0x00;
	buf[2]=0x5e;
	buf[5]=addr&0xFF;
	addr>>=8;
	buf[4]=addr&0xFF;
	addr>>=8;
	buf[3]=addr&0x7F;
}

/*
 *	Map a multicast IP onto multicast MAC for type IP-over-InfiniBand.
 *	Leave P_Key as 0 to be filled in by driver.
 */

static inline void ip_ib_mc_map(u32 addr, char *buf)
{
	buf[0]  = 0;		/* Reserved */
	buf[1]  = 0xff;		/* Multicast QPN */
	buf[2]  = 0xff;
	buf[3]  = 0xff;
	addr    = ntohl(addr);
	buf[4]  = 0xff;
	buf[5]  = 0x12;		/* link local scope */
	buf[6]  = 0x40;		/* IPv4 signature */
	buf[7]  = 0x1b;
	buf[8]  = 0;		/* P_Key */
	buf[9]  = 0;
	buf[10] = 0;
	buf[11] = 0;
	buf[12] = 0;
	buf[13] = 0;
	buf[14] = 0;
	buf[15] = 0;
	buf[19] = addr & 0xff;
	addr  >>= 8;
	buf[18] = addr & 0xff;
	addr  >>= 8;
	buf[17] = addr & 0xff;
	addr  >>= 8;
	buf[16] = addr & 0x0f;
}

#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
#include <linux/ipv6.h>
#endif

static __inline__ void inet_reset_saddr(struct sock *sk)
{
	inet_sk(sk)->rcv_saddr = inet_sk(sk)->saddr = 0;
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	if (sk->sk_family == PF_INET6) {
		struct ipv6_pinfo *np = inet6_sk(sk);

		memset(&np->saddr, 0, sizeof(np->saddr));
		memset(&np->rcv_saddr, 0, sizeof(np->rcv_saddr));
	}
#endif
}

#endif

extern int	ip_call_ra_chain(struct sk_buff *skb);

/*
 *	Functions provided by ip_fragment.o
 */

enum ip_defrag_users
{
	IP_DEFRAG_LOCAL_DELIVER,
	IP_DEFRAG_CALL_RA_CHAIN,
	IP_DEFRAG_CONNTRACK_IN,
	IP_DEFRAG_CONNTRACK_OUT,
	IP_DEFRAG_NAT_OUT,
	IP_DEFRAG_VS_IN,
	IP_DEFRAG_VS_OUT,
	IP_DEFRAG_VS_FWD
};

struct sk_buff *ip_defrag(struct sk_buff *skb, u32 user);
extern int ip_frag_nqueues;
extern atomic_t ip_frag_mem;

/*
 *	Functions provided by ip_forward.c
 */
 
extern int ip_forward(struct sk_buff *skb);
extern int ip_net_unreachable(struct sk_buff *skb);
 
/*
 *	Functions provided by ip_options.c
 */
 
extern void ip_options_build(struct sk_buff *skb, struct ip_options *opt, u32 daddr, struct rtable *rt, int is_frag);
extern int ip_options_echo(struct ip_options *dopt, struct sk_buff *skb);
extern void ip_options_fragment(struct sk_buff *skb);
extern int ip_options_compile(struct ip_options *opt, struct sk_buff *skb);
extern int ip_options_get(struct ip_options **optp, unsigned char *data, int optlen, int user);
extern void ip_options_undo(struct ip_options * opt);
extern void ip_forward_options(struct sk_buff *skb);
extern int ip_options_rcv_srr(struct sk_buff *skb);

/*
 *	Functions provided by ip_sockglue.c
 */

extern void	ip_cmsg_recv(struct msghdr *msg, struct sk_buff *skb);
extern int	ip_cmsg_send(struct msghdr *msg, struct ipcm_cookie *ipc);
extern int	ip_setsockopt(struct sock *sk, int level, int optname, char __user *optval, int optlen);
extern int	ip_getsockopt(struct sock *sk, int level, int optname, char __user *optval, int __user *optlen);
extern int	ip_ra_control(struct sock *sk, unsigned char on, void (*destructor)(struct sock *));

extern int 	ip_recv_error(struct sock *sk, struct msghdr *msg, int len);
extern void	ip_icmp_error(struct sock *sk, struct sk_buff *skb, int err, 
			      u16 port, u32 info, u8 *payload);
extern void	ip_local_error(struct sock *sk, int err, u32 daddr, u16 dport,
			       u32 info);


#else
#include <net/ip.h>
#endif

#endif

