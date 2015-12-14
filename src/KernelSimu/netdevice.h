////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: netdevice.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelSimu_aosNetdevice_h
#define aos_KernelSimu_aosNetdevice_h

#ifdef AOS_KERNEL_SIMULATE

#include "KernelSimu/aosKernelMutex.h"
#include "KernelSimu/if.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/list.h"
#include "KernelSimu/device.h"
#include "KernelSimu/kobject.h"
#include "KernelSimu/compiler.h"

#include "Porting/Mutex.h"
#define ETH_ALEN    6
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */

/*
 *  *	These are the defined Ethernet Protocol ID's.
 *   */

#define ETH_P_LOOP	0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_PUP	0x0200		/* Xerox PUP packet		*/
#define ETH_P_PUPAT	0x0201		/* Xerox PUP Addr Trans packet	*/
#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_X25	0x0805		/* CCITT X.25			*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define	ETH_P_BPQ	0x08FF		/* G8BPQ AX.25 Ethernet Packet	[ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_IEEEPUP	0x0a00		/* Xerox IEEE802.3 PUP packet */
#define ETH_P_IEEEPUPAT	0x0a01		/* Xerox IEEE802.3 PUP Addr Trans packet */
#define ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define ETH_P_LAT       0x6004          /* DEC LAT                      */
#define ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define ETH_P_CUST      0x6006          /* DEC Customer use             */
#define ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define ETH_P_RARP      0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_ATALK	0x809B		/* Appletalk DDP		*/
#define ETH_P_AARP	0x80F3		/* Appletalk AARP		*/
#define ETH_P_8021Q	0x8100          /* 802.1Q VLAN Extended Header  */
#define ETH_P_IPX	0x8137		/* IPX over DIX			*/
#define ETH_P_IPV6	0x86DD		/* IPv6 over bluebook		*/
#define ETH_P_WCCP	0x883E		/* Web-cache coordination protocol
								   					 * defined in draft-wilson-wrec-wccp-v2-00.txt */
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/
#define ETH_P_MPLS_UC	0x8847		/* MPLS Unicast traffic		*/
#define ETH_P_MPLS_MC	0x8848		/* MPLS Multicast traffic	*/
#define ETH_P_ATMMPOA	0x884c		/* MultiProtocol Over ATM	*/
#define ETH_P_ATMFATE	0x8884		/* Frame-based ATM Transport
									   					 * over Ethernet
														 * 					 */
#define ETH_P_EDP2	0x88A2		/* Coraid EDP2			*/

/*
 *  *	Non DIX types. Won't clash for 1500 types.
 *   */
 
#define ETH_P_802_3	0x0001		/* Dummy type for 802.3 frames  */
#define ETH_P_AX25	0x0002		/* Dummy protocol id for AX.25  */
#define ETH_P_ALL	0x0003		/* Every packet (be careful!!!) */
#define ETH_P_802_2	0x0004		/* 802.2 frames 		*/
#define ETH_P_SNAP	0x0005		/* Internal only		*/
#define ETH_P_DDCMP     0x0006          /* DEC DDCMP: Internal only     */
#define ETH_P_WAN_PPP   0x0007          /* Dummy type for WAN PPP frames*/
#define ETH_P_PPP_MP    0x0008          /* Dummy type for PPP MP frames */
#define ETH_P_LOCALTALK 0x0009		/* Localtalk pseudo type 	*/
#define ETH_P_PPPTALK	0x0010		/* Dummy type for Atalk over PPP*/
#define ETH_P_TR_802_2	0x0011		/* 802.2 frames 		*/
#define ETH_P_MOBITEX	0x0015		/* Mobitex (kaz@cafe.net)	*/
#define ETH_P_CONTROL	0x0016		/* Card specific control frames */
#define ETH_P_IRDA	0x0017		/* Linux-IrDA			*/
#define ETH_P_ECONET	0x0018		/* Acorn Econet			*/
#define ETH_P_HDLC	0x0019		/* HDLC frames			*/

#define HAVE_ALLOC_NETDEV		/* feature macro: alloc_xxxdev
					   functions are available. */
#define HAVE_FREE_NETDEV		/* free_netdev() */
#define HAVE_NETDEV_PRIV		/* netdev_priv() */

#define NET_XMIT_SUCCESS	0
#define NET_XMIT_DROP		1	/* skb dropped			*/
#define NET_XMIT_CN		2	/* congestion notification	*/
#define NET_XMIT_POLICED	3	/* skb is shot by police	*/
#define NET_XMIT_BYPASS		4	/* packet does not leave via dequeue;
					   (TC use only - dev_queue_xmit
					   returns this as NET_XMIT_SUCCESS) */

/* Backlog congestion levels */
#define NET_RX_SUCCESS		0   /* keep 'em coming, baby */
#define NET_RX_DROP		1  /* packet dropped */
#define NET_RX_CN_LOW		2   /* storm alert, just in case */
#define NET_RX_CN_MOD		3   /* Storm on its way! */
#define NET_RX_CN_HIGH		4   /* The storm is here */
#define NET_RX_BAD		5  /* packet dropped due to kernel error */

#define net_xmit_errno(e)	((e) != NET_XMIT_CN ? -ENOBUFS : 0)

#define MAX_ADDR_LEN	32		/* Largest hardware address length */

/* Driver transmit return codes */
#define NETDEV_TX_OK 0		/* driver took care of packet */
#define NETDEV_TX_BUSY 1	/* driver tx path was busy*/
#define NETDEV_TX_LOCKED -1	/* driver tx lock was already taken */

/*
 *	Compute the worst case header length according to the protocols
 *	used.
 */
 
#if !defined(CONFIG_AX25) && !defined(CONFIG_AX25_MODULE) && !defined(CONFIG_TR)
#define LL_MAX_HEADER	32
#else
#if defined(CONFIG_AX25) || defined(CONFIG_AX25_MODULE)
#define LL_MAX_HEADER	96
#else
#define LL_MAX_HEADER	48
#endif
#endif

#if !defined(CONFIG_NET_IPIP) && \
    !defined(CONFIG_IPV6) && !defined(CONFIG_IPV6_MODULE)
#define MAX_HEADER LL_MAX_HEADER
#else
#define MAX_HEADER (LL_MAX_HEADER + 48)
#endif

/*
 *	Network device statistics. Akin to the 2.0 ether stats but
 *	with byte counters.
 */
 
struct net_device_stats
{
	unsigned long	rx_packets;		/* total packets received	*/
	unsigned long	tx_packets;		/* total packets transmitted	*/
	unsigned long	rx_bytes;		/* total bytes received 	*/
	unsigned long	tx_bytes;		/* total bytes transmitted	*/
	unsigned long	rx_errors;		/* bad packets received		*/
	unsigned long	tx_errors;		/* packet transmit problems	*/
	unsigned long	rx_dropped;		/* no space in linux buffers	*/
	unsigned long	tx_dropped;		/* no space available in linux	*/
	unsigned long	multicast;		/* multicast packets received	*/
	unsigned long	collisions;

	/* detailed rx_errors: */
	unsigned long	rx_length_errors;
	unsigned long	rx_over_errors;		/* receiver ring buff overflow	*/
	unsigned long	rx_crc_errors;		/* recved pkt with crc error	*/
	unsigned long	rx_frame_errors;	/* recv'd frame alignment error */
	unsigned long	rx_fifo_errors;		/* recv'r fifo overrun		*/
	unsigned long	rx_missed_errors;	/* receiver missed packet	*/

	/* detailed tx_errors */
	unsigned long	tx_aborted_errors;
	unsigned long	tx_carrier_errors;
	unsigned long	tx_fifo_errors;
	unsigned long	tx_heartbeat_errors;
	unsigned long	tx_window_errors;
	
	/* for cslip etc */
	unsigned long	rx_compressed;
	unsigned long	tx_compressed;
};


/* Media selection options. */
enum {
        IF_PORT_UNKNOWN = 0,
        IF_PORT_10BASE2,
        IF_PORT_10BASET,
        IF_PORT_AUI,
        IF_PORT_100BASET,
        IF_PORT_100BASETX,
        IF_PORT_100BASEFX
};

struct net_device_stats;
struct iw_statistics;
struct iw_handler_def;
struct iw_public_data;
struct ethtool_ops;
struct dev_mc_list;
struct timer_list;
struct list_head;
struct Qdisc;
struct hlist_node;
struct net_bridge_port;
struct divert_blk;
struct class_device;

struct net_device
{

	char			name[IFNAMSIZ];

	unsigned long		mem_end;	/* shared mem end	*/
	unsigned long		mem_start;	/* shared mem start	*/
	unsigned long		base_addr;	/* device I/O address	*/
	unsigned int		irq;		/* device IRQ number	*/

	unsigned char		if_port;	/* Selectable AUI, TP,..*/
	unsigned char		dma;		/* DMA channel		*/

	unsigned long		state;

	struct net_device	*next;
	
	int			(*init)(struct net_device *dev);

	struct net_device	*next_sched;

	int			ifindex;
	int			iflink;


	struct net_device_stats* 	(*get_stats)(struct net_device *dev);
	struct iw_statistics*		(*get_wireless_stats)(struct net_device *dev);

	const struct iw_handler_def *	wireless_handlers;
	struct iw_public_data *		wireless_data;
	struct ethtool_ops *		ethtool_ops;

	unsigned long				trans_start;
	unsigned long				last_rx;	/* Time of last Rx	*/
	unsigned short				flags;	/* interface flags (a la BSD)	*/
	unsigned short				gflags;
    unsigned short          	priv_flags; 
    unsigned short          	unused_alignment_fixer;
	unsigned					mtu;
	unsigned short				type;	/* interface hardware type	*/
	unsigned short				hard_header_len;	/* hardware hdr length	*/
	void					   *priv;	/* pointer to private data	*/
	struct net_device		   *master; 

	unsigned char				broadcast[MAX_ADDR_LEN];	/* hw bcast add	*/
	unsigned char				dev_addr[MAX_ADDR_LEN];	/* hw address	*/
	unsigned char				addr_len;	/* hardware address length	*/
	unsigned short          	dev_id;		/* for shared network cards */

	struct dev_mc_list		   *mc_list;	/* Multicast mac addresses	*/
	int							mc_count;	/* Number of installed mcasts	*/
	int							promiscuity;
	int							allmulti;

	int							watchdog_timeo;
	struct timer_list			watchdog_timer;

	void 					   *atalk_ptr;	/* AppleTalk link 	*/
	void					   *ip_ptr;	/* IPv4 specific data	*/  
	void                       *dn_ptr;        /* DECnet specific data */
	void                       *ip6_ptr;       /* IPv6 specific data */
	void					   *ec_ptr;	/* Econet specific data	*/
	void					   *ax25_ptr;	/* AX.25 specific data */

	struct list_head	poll_list;	/* Link to poll list	*/
	int			quota;
	int			weight;

	struct Qdisc			   *qdisc;
	struct Qdisc			   *qdisc_sleeping;
	struct Qdisc			   *qdisc_ingress;
	struct list_head			qdisc_list;
	unsigned long				tx_queue_len;

	OmnMutexType				ingress_lock;
	OmnMutexType				xmit_lock;
	int							xmit_lock_owner;
	OmnMutexType				queue_lock;
	aosKernelMutex				refcntLock;
	int							refcnt;
	struct list_head			todo_list;
	struct hlist_node			name_hlist;
	struct hlist_node			index_hlist;

	enum { NETREG_UNINITIALIZED=0,
	       NETREG_REGISTERING,	/* called register_netdevice */
	       NETREG_REGISTERED,	/* completed register todo */
	       NETREG_UNREGISTERING,	/* called unregister_netdevice */
	       NETREG_UNREGISTERED,	/* completed unregister todo */
	       NETREG_RELEASED,		/* called free_netdev */
	} reg_state;

	int			features;
#define NETIF_F_SG		1	/* Scatter/gather IO. */
#define NETIF_F_IP_CSUM		2	/* Can checksum only TCP/UDP over IPv4. */
#define NETIF_F_NO_CSUM		4	/* Does not require checksum. F.e. loopack. */
#define NETIF_F_HW_CSUM		8	/* Can checksum all the packets. */
#define NETIF_F_HIGHDMA		32	/* Can DMA to high memory. */
#define NETIF_F_FRAGLIST	64	/* Scatter/gather IO. */
#define NETIF_F_HW_VLAN_TX	128	/* Transmit VLAN hw acceleration */
#define NETIF_F_HW_VLAN_RX	256	/* Receive VLAN hw acceleration */
#define NETIF_F_HW_VLAN_FILTER	512	/* Receive filtering on VLAN */
#define NETIF_F_VLAN_CHALLENGED	1024	/* Device cannot handle VLAN packets */
#define NETIF_F_TSO		2048	/* Can offload TCP/IP segmentation */
#define NETIF_F_LLTX		4096	/* LockLess TX */

	void		(*uninit)(struct net_device *dev);
	void		(*destructor)(struct net_device *dev);
	int			(*open)(struct net_device *dev);
	int			(*stop)(struct net_device *dev);
	int			(*hard_start_xmit) (struct sk_buff *skb,
						    struct net_device *dev);
#define HAVE_NETDEV_POLL
	int			(*poll) (struct net_device *dev, int *quota);
	int			(*hard_header) (struct sk_buff *skb,
						struct net_device *dev,
						unsigned short type,
						void *daddr,
						void *saddr,
						unsigned len);
	int			(*rebuild_header)(struct sk_buff *skb);
#define HAVE_MULTICAST			 
	void			(*set_multicast_list)(struct net_device *dev);
#define HAVE_SET_MAC_ADDR  		 
	int			(*set_mac_address)(struct net_device *dev,
						   void *addr);
#define HAVE_PRIVATE_IOCTL
	int			(*do_ioctl)(struct net_device *dev,
					    struct ifreq *ifr, int cmd);
#define HAVE_SET_CONFIG
	int			(*set_config)(struct net_device *dev,
					      struct ifmap *map);
#define HAVE_HEADER_CACHE
	int			(*hard_header_cache)(struct neighbour *neigh,
						     struct hh_cache *hh);
	void		(*header_cache_update)(struct hh_cache *hh,
						       struct net_device *dev,
						       unsigned char *  haddr);
#define HAVE_CHANGE_MTU
	int			(*change_mtu)(struct net_device *dev, int new_mtu);

#define HAVE_TX_TIMEOUT
	void		(*tx_timeout) (struct net_device *dev);

	void		(*vlan_rx_register)(struct net_device *dev,
						    struct vlan_group *grp);
	void		(*vlan_rx_add_vid)(struct net_device *dev,
						   unsigned short vid);
	void		(*vlan_rx_kill_vid)(struct net_device *dev,
						    unsigned short vid);

	int			(*hard_header_parse)(struct sk_buff *skb,
						     unsigned char *haddr);
	int			(*neigh_setup)(struct net_device *dev, struct neigh_parms *);
	int			(*accept_fastpath)(struct net_device *, struct dst_entry*);
#ifdef CONFIG_NETPOLL
	int			netpoll_rx;
#endif
#ifdef CONFIG_NET_POLL_CONTROLLER
	void                    (*poll_controller)(struct net_device *dev);
#endif

	struct net_bridge_port	*br_port;

#ifdef CONFIG_NET_DIVERT
	struct divert_blk	*divert;
#endif /* CONFIG_NET_DIVERT */

	struct class_device	class_dev;
	int padded;
};

static inline void dev_put(struct net_device *dev)
{
	OmnMutexLock(dev->refcntLock);
	(dev->refcnt)--;
	OmnMutexUnlock(dev->refcntLock);
}

extern net_device *netdevice_create(const char *name, int index);

#define dev_hold(dev) \
	aosKernelLock(dev->refcntLock); \
	((dev)->refcnt)++; \
	aosKernelUnlock(dev->refcntLock);

struct hh_cache
{
	struct hh_cache *hh_next;
	OmnMutexType	hh_refcntLock;
	int				hh_refcnt;
	unsigned short  hh_type;	
	int				hh_len;		
	int		(*hh_output)(struct sk_buff *skb);
	OmnMutexType	hh_lock;

#define HH_DATA_MOD	16
#define HH_DATA_OFF(__len) \
	(HH_DATA_MOD - ((__len) & (HH_DATA_MOD - 1)))
#define HH_DATA_ALIGN(__len) \
	(((__len)+(HH_DATA_MOD-1))&~(HH_DATA_MOD - 1))
	unsigned long	hh_data[HH_DATA_ALIGN(LL_MAX_HEADER) / sizeof(long)];
};


#define LL_RESERVED_SPACE(dev) dev->hard_header_len
#define LL_RESERVED_SPACE_EXTRA(dev,extra) \
	((((dev)->hard_header_len+extra)&~(HH_DATA_MOD - 1)) + HH_DATA_MOD)


extern struct net_device	*dev_get_by_name(const char *name);

#else
#include <linux/netdevice.h>
#endif

#endif


