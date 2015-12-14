////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSkb.cpp
// Description:
//	1. How to creaet an skb
//		aosAllocSkb(datalen);
//		aosInsertTcpHdr(skb, saddr, daddr, sport, dport, 
//			datalen, hdrlen, checksum);
//		aosInsertIPHdr(skb, saddr, daddr, opt, identity, 
//			protocol, tos, priority);   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/aosSkb.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/socket.h>
#include <KernelSimu/netdevice.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/snmp.h>
#include <KernelSimu/ip_output.h>
#include <KernelSimu/netfilter.h>
#include <KernelSimu/netfilter_ipv4.h>
#include <KernelSimu/udp.h>
#include <KernelSimu/route.h>
#include <KernelSimu/dst.h>
#include <KernelSimu/ip.h>
#include <KernelSimu/net_ip.h>
#include <KernelSimu/checksum.h>
#include <KernelSimu/netinet_in.h>
#include <KernelSimu/errno.h>
#include <KernelSimu/stddef.h>
#include <KernelSimu/tcp.h>
#include <KernelSimu/in.h>
#include <KernelSimu/string.h>

// #include "aos/aosKernelError.h"


// 
// It creates an SKB with no room for IP options in Layer 4 and Layer 3
// headers. 
//
int aosAllocSkb(u32 datalen)
{
	return aosAllocL4Skb(datalen, 5, 5, 14);
}


// 
// This is a normal skb creation function except that it will reserve the space
// for tcphdr. If it fails the memory allocation, an alarm is raised and null
// is returned. Otherwise, it will reserve the room for TCP header, IP header, 
// and MAC header. 
// 
int aosAllocL4Skb(u32 datalen, 
				  u32 layer4len, 
				  u32 layer3len, 
				  u32 layer2len)
{
	struct sk_buff *skb = alloc_skb(datalen + layer2len + layer3len + 
		layer4len + 15, GFP_KERNEL);

	if (!skb)
	{
		aosAlarmRated(eAosAlarm_aosAllocTcpSkb, 1, datalen, layer4len, 0, 0);
		return 0;
	}

	// 
	// Reserve room for the headers
	//
	skb_reserve(skb, layer2len + layer3len + layer4len);

	// 
	// Make room for the data portion.
	//
	skb_put(skb, datalen);
	return 0;
}


int aosCreateSkbs(void *from, 
			   unsigned int length, 
			   int transhdrlen,
			   struct rtable *rt,
			   struct ip_options *opt,
			   unsigned short dport,
			   unsigned int flags, 
			   struct sk_buff_head *skbChain)
{
	struct sk_buff *skb;

	int hh_len;
	int exthdrlen = 0;
	unsigned int mtu = 0;
	int fragsize;
	int copy;
	int err;
	int offset = 0;
	unsigned int maxfraglen, fragheaderlen;
	int csummode = CHECKSUM_NONE;
	unsigned int datalen;
	unsigned int fraglen;
	unsigned int alloclen;
	unsigned char *data;

	if (flags&MSG_PROBE)
	{
		return 0;
	}


	fragsize = mtu = dst_pmtu(&rt->u.dst);

	/*
	if (opt) 
	{
		if ((exthdrlen = rt->u.dst.header_len) != 0) 
		{
			length += exthdrlen;
			transhdrlen += exthdrlen;
		}
	} 
	*/

	hh_len = LL_RESERVED_SPACE(rt->u.dst.dev);

	fragheaderlen = sizeof(struct iphdr) + (opt ? opt->optlen : 0);
	maxfraglen = ((mtu - fragheaderlen) & ~7) + fragheaderlen;

	if (length > 0xFFFF - fragheaderlen) 
	{
		// ip_local_error(sk, EMSGSIZE, rt->rt_dst, dport, mtu-exthdrlen);
		return -EMSGSIZE;
	}

	/*
	 * transhdrlen > 0 means that this is the first fragment and we wish
	 * it won't be fragmented in the future.
	 */
	// 
	// If 'transhdrlen' == 0, it is not the first skb, or the UDP datagram is
	// fragmented. For fragmented skbs, checksum has been done by this function
	// even the hardware can do the checksum. 
	// 
	// It is for the same reason that the total length of the datagram must be
	// less than mtu in order to take the advantages of hardware checksum. 
	//
	// Issue: not sure why 'exthdrlen' must be 0 in order to let hardware do 
	// the checksum.
	//
	// NETIF_F_NO_CSUM:		No need to do the checksum
	// NETIF_F_HW_CSUM:		The hardware can do the checksum
	// NETIF_F_IP_CSUM:		???
	//
	// Note that csummode is set to CHECKSUM_NONE by default, which means that
	// we have done the checksum already when building the skbs. 
	//
	if (transhdrlen &&
	    length + fragheaderlen <= mtu &&
	    rt->u.dst.dev->features&(NETIF_F_IP_CSUM|NETIF_F_NO_CSUM|NETIF_F_HW_CSUM) &&
	    !exthdrlen)
	{
		csummode = CHECKSUM_HW;
	}

	/* So, what's going on in the loop below?
	 *
	 * We use calculated fragment length to generate chained skb,
	 * each of segments is IP fragment ready for sending to network after
	 * adding appropriate IP header.
	 * 
	 * What is the value of 'length'? Does it include transhdrlen?
	 */
	while (length > 0) 
	{
		skb = 0;

		/*
		 * If remaining data exceeds the mtu,
		 * we know we need more fragment(s).
		 */
		datalen = length;

		if (datalen > mtu - fragheaderlen)
		{
			//
			// maxfraglen is the rounded down maximum fragment length, 
			// which is (mtu - fraghdrlen)&~7 + fraghdrlen. So datalen
			// is (mtu - fraghdrlen)&~7 now. This means that datalen
			// is aligned at the boundary. When calculating:
			//	fraggap = skb_prev->len - maxfraglen
			// it should be 0. The only possibility for fraggap not to be
			// is the last fragment.
			// 
			datalen = maxfraglen - fragheaderlen;
		}

		fraglen = datalen + fragheaderlen;

		if ((flags & MSG_MORE) && 
		    !(rt->u.dst.dev->features&NETIF_F_SG))
		{
			// CD: Don't know why we need to do this??????
			alloclen = mtu;
		}
		else
		{
			alloclen = datalen + fragheaderlen;
			// CD: 'datalen' is the size we can stuff for this packet. 
			// It is min('length + fraggap', 'maxfraglen - fragheaderlen').
			// In other word, we want to stuff as much as possible.
		}

		/* The last fragment gets additional space at tail.
		 * Note, with MSG_MORE we overallocate on fragments,
		 * because we have no idea what fragment will be
		 * the last.
		 */
		if (datalen == length)
		{
			alloclen += rt->u.dst.trailer_len;
		}

		skb = alloc_skb(alloclen + hh_len + 15, GFP_KERNEL);

		if (unlikely(skb == NULL))
		{
			err = -ENOBUFS;
			goto error;
		}

		// 
		// Set the dst_entry to the skb, and increment the reference counter
		// in the pointed to dst_entry. When the skb is sent out, the reference
		// will be decremented. 
		//
		// skb->dst = dst_clone(&rt->u.dst);

		/*
		 *	Fill in the control structures
		 */
		skb->ip_summed = csummode;
		skb->csum = 0;

		// CD: This space is for Layer 2 header
		skb_reserve(skb, hh_len);

		/*
		 *	Find where to start putting bytes.
		 */
		// CD: Make space for the data. Note 'fraglen = datalen+fragheaderlen'.
		// 'fragheaderlen' = iphdr length. If it is the first skb, datalen 
		// contains the transhdrlen. 'data' will point to the beginning of the
		// room being allocated by this funciton.
		//
		data = skb_put(skb, fraglen);

		// 
		// Let the network header (skb->nh.raw) points to 'data'. It then
		// moves 'data' 'fragheaderlen'. This is to make room for the fragment
		// header, whose length is 'fragheaderlen'.
		//
		// CD: Appears that the ext header will be in front of the standard
		// network header. What is 'ext header'?
		skb->nh.raw = data + exthdrlen;
		data += fragheaderlen;

		// 
		// The Layer 4 header follows the Layer 3 header. 
		//
		skb->h.raw = data + exthdrlen;

		// 
		// Copy the payload data
		//
		copy = datalen - transhdrlen;
		if (copy > 0 && aosCopyDataToSkb((unsigned char *)from, data + transhdrlen, 
			offset, copy, skb) < 0) 
		{
			err = -EFAULT;
			kfree_skb(skb);
			goto error;
		}

		offset += copy;
		length -= datalen;
		transhdrlen = 0;
		exthdrlen = 0;
		csummode = CHECKSUM_NONE;

		__skb_queue_tail(skbChain, skb);

		// 
		// Put the skb into the queue.
		//
		continue;
	}

	// 
	// Now we have created all the skbs for the datagram. The skbs are chained
	// and the chain head is 'skb'. Next, we need to calculate the UDP header. 
	// 
	return 0;

error:
	IP_INC_STATS(IPSTATS_MIB_OUTDISCARDS);
	return err; 
}


// 
// This function assumes the IP header has not been inserted into the skb
// yet. Otherwise, it is an error.
//
int aosInsertIPHdr(struct sk_buff *skb, 
				   u32 saddr,
				   u32 daddr, 
				   struct ip_options *opt, 
				   struct dst_entry *dst,
				   int identity, 
				   int protocol,
				   int tos,
				   int priority)
{
	u32 hdrlen = 20;
	if (opt) 
	{
		// 
		// If 'opt' is not null, we need to add the option length to the
		// header length. Currently, this is not implemented yet.
		// 
		// iph->ihl += opt->optlen>>2;
		// ip_options_build(skb, opt, inet->cork.addr, rt, 0);
	}

	// 
	// Insert the header into the skb
	//
	skb_push(skb, hdrlen);
	return aosSetIPHdr(skb, saddr, daddr, opt, dst, identity, protocol, tos, priority);
}


// 
// This function inserts a TCP header in front of the skb. It assumes
// the header has not been inserted yet. This function does not verify it.
// If "doChecksum" is true, checksum will be calculated. 
//
// 'datalen': the entire UDP datagram data length (not including the header length)
//
int aosInsertTcpHdr(struct sk_buff *skb,
				 u32 tcphdrlen,
				 u32 saddr, 
				 u32 daddr,
				 u16 sport,
				 u16 dport,
				 u16 datalen, 
				 u16 seq,
				 u16 ackseq,
				 u16 doff,
				 u8  fin,
				 u8  ack,
				 u8  syn,
				 u8  rst,
				 u8  urg,
				 u8  ece,
				 u8  cwr,
				 u16 window,
				 u16 urgptr,
				 u8  doChecksum)
{
	skb_push(skb, tcphdrlen);
	return aosSetTcpHdr(skb, tcphdrlen, saddr, daddr, sport, dport, 
		datalen, seq, ackseq, doff, fin, ack, syn, rst, urg, ece, cwr,
		window, urgptr, doChecksum);
}


// 
// This function inserts a UDP header in front of the skb. It assumes
// the header has not been inserted yet. This function does not verify it.
// If "doChecksum" is true, checksum will be calculated. 
//
int aosInsertUdpHdr(struct sk_buff *skb,
					u32 saddr, 
					u32 daddr,
					u16 sport,
					u16 dport,
					u16 datalen, 
					u16 hdrlen,
					int doChecksum)
{
	struct sk_buff_head skbChain;
	skb_push(skb, hdrlen);

	skb_queue_head_init(&skbChain);
	__skb_queue_tail(&skbChain, skb);

	return aosSetUdpHdr(&skbChain, saddr, daddr, sport, dport, 
		datalen, hdrlen, doChecksum);
}


// 
// 'datalen': the entire UDP datagram data length (not including the 
// header length)
//
// This function assumes 'skbs' is a list of skbs for a UDP datagram.
// Only the first skb in the list needs the UDP header. It assumes 
// the first skb has already set the pointers properly for the udp
// header (i.e., skb->h.uh points to the udp header). Data checksum
// has already been calculated. All it does is to set the sport, dport,
// length, and checksum. Note that the length contains the lengths of
// both the udp header and the data. 
//
int aosSetUdpHdr(struct sk_buff_head *skbs,
					unsigned int   transhdrlen,
					unsigned int   saddr, 
					unsigned int   daddr,
					unsigned short sport,
					unsigned short dport,
					unsigned short datalen, 
					int doChecksum)
{
	struct sk_buff *skb = skb_peek(skbs);
	struct udphdr *uh;

	if (skb == NULL)
	{
		return 0;
	}

	uh = skb->h.uh;
	uh->source = htons(sport);
	uh->dest = htons(dport);
	uh->len = htons(datalen + transhdrlen);
	uh->check = 0;

	if (!doChecksum) 
	{
		skb->ip_summed = CHECKSUM_NONE;
		return 0;
	}

	if (skb_queue_len(skbs) == 1) 
	{
		/*
		 * Only one fragment on the socket.
		 */
		if (skb->ip_summed == CHECKSUM_HW) 
		{
			skb->csum = offsetof(struct udphdr, check);
			uh->check = ~csum_tcpudp_magic(saddr, daddr,
					uh->len, IPPROTO_UDP, 0);
		} 
		else 
		{
			// 
			// Calculate the UDP header checksum. UDP checksum
			// consists of three parts: 
			// 	UDP header checksum
			//  UDP data checksum
			//  Pseudo Header checksum
			// 
			// The data checksum has (assumed) already been calculated
			// (in skb->csum). The following calculate the remaining
			// two.
			//
			skb->csum = csum_partial((const unsigned char *)uh,
					sizeof(struct udphdr), skb->csum);

			// 
			// Calculate the pseudo header checksum. Pseudo header
			// contains saddr, daddr, protocol, and udp length.
			//
			uh->check = csum_tcpudp_magic(saddr, daddr,
					datalen + transhdrlen, IPPROTO_UDP, skb->csum);

			//
			// Checksum cannot be 0. If it is, set it to -1.
			//
			if (uh->check == 0)
			{
				uh->check = (u16)-1;
			}
		}
	} 
	else 
	{
		unsigned int csum = 0;
		/*
		 * HW-checksum won't work as there are two or more 
		 * fragments on the socket so that all csums of sk_buffs
		 * should be together.
		 */
		if (skb->ip_summed == CHECKSUM_HW) 
		{
			int offset = (unsigned char *)uh - skb->data;
			skb->csum = skb_checksum(skb, offset, skb->len - offset, 0);

			skb->ip_summed = CHECKSUM_NONE;
		} 
		else 
		{
			skb->csum = csum_partial((const unsigned char *)uh,
					sizeof(struct udphdr), skb->csum);
		}

		skb_queue_walk(skbs, skb) 
		{
			csum = csum_add(csum, skb->csum);
		}
		
		uh->check = csum_tcpudp_magic(saddr, daddr,
				uh->len, IPPROTO_UDP, csum);

		//
		// Checksum cannot be 0. If it is, set it to -1.
		//
		if (uh->check == 0)
		{
			uh->check = (u16)-1;
		}
	}

	return 0;
}


// 
// 'datalen': the entire UDP datagram data length (not including the 
// header length)
//
// This function assumes 'skbs' is a list of skbs for a UDP datagram.
// Only the first skb in the list needs the UDP header. It assumes 
// the first skb has already set the pointers properly for the udp
// header (i.e., skb->h.uh points to the udp header). Data checksum
// has already been calculated. All it does is to set the sport, dport,
// length, and checksum. Note that the length contains the lengths of
// both the udp header and the data. 
//
int aosSetTcpHdr(struct sk_buff *skb,
				 u32 tcphdrlen,
				 u32 saddr, 
				 u32 daddr,
				 u16 sport,
				 u16 dport,
				 u16 datalen, 
				 u16 seq,
				 u16 ackSeq,
				 u16 doff,
				 u8  fin,
				 u8  ack,
				 u8  syn,
				 u8  rst,
				 u8  urg,
				 u8  ece,
				 u8  cwr,
				 u16 window,
				 u16 urgptr,
				 u8  doChecksum)
{
	struct tcphdr *th;

	if (skb == NULL)
	{
		return 0;
	}

	th = skb->h.th;
	th->source = htons(sport);
	th->dest = htons(dport);
	th->seq = htons(seq);
	th->ack_seq = htons(ackSeq);
	th->doff = htons(doff);
	th->fin = fin;
	th->ack = ack;
	th->syn = syn;
	th->rst = rst;
	th->urg = urg;
	th->ece = ece;
	th->cwr = cwr;
	th->window = htons(window);
	th->urg_ptr = urgptr;

	th->check = 0;

	if (!doChecksum) 
	{
		skb->ip_summed = CHECKSUM_NONE;
		return 0;
	}

	/*
	Not implemented yet
	if (skb_queue_len(skbs) == 1) 
	{
		//
		// Only one fragment on the socket.
		//
		if (skb->ip_summed == CHECKSUM_HW) 
		{
			//
			// Don't know how to process it yet. Chen Ding, 06/08/2005
			//
			// skb->csum = offsetof(struct tcphdr, check);
			// th->check = ~csum_tcpudp_magic(saddr, daddr,
			// 		tcplen, IPPROTO_UDP, 0);
		} 
		else 
		{
			// 
			// Calculate the TCP header checksum. TCP checksum
			// consists of three parts: 
			// 	TCP header checksum
			//  TCP data checksum
			//  Pseudo Header checksum
			// 
			// The data checksum has (assumed) already been calculated
			// (in skb->csum). The following calculate the remaining
			// two.
			//
			skb->csum = csum_partial((const unsigned char *)th,
					sizeof(struct tcphdr), skb->csum);

			// 
			// Calculate the pseudo header checksum. Pseudo header
			// contains saddr, daddr, protocol, and tcp length.
			//
			uh->check = csum_tcpudp_magic(saddr, daddr,
					datalen + tcphdrlen, IPPROTO_UDP, skb->csum);

			//
			// Checksum cannot be 0. If it is, set it to -1.
			//
			if (uh->check == 0)
			{
				uh->check = -1;
			}
		}
	} 
	else 
	{
		unsigned int csum = 0;
		//
		// HW-checksum won't work as there are two or more 
		// fragments on the socket so that all csums of sk_buffs
		// should be together.
		//
		if (skb->ip_summed == CHECKSUM_HW) 
		{
			// int offset = (unsigned char *)uh - skb->data;
			// skb->csum = skb_checksum(skb, offset, skb->len - offset, 0);

			// skb->ip_summed = CHECKSUM_NONE;
		} 
		else 
		{
			skb->csum = csum_partial((const unsigned char *)th,
					sizeof(struct tcphdr), skb->csum);
		}

		skb_queue_walk(skbs, skb) 
		{
			csum = csum_add(csum, skb->csum);
		}
		
		th->check = csum_tcpudp_magic(saddr, daddr,
				datalen + tcphdrlen, IPPROTO_UDP, csum);

		//
		// Checksum cannot be 0. If it is, set it to -1.
		//
		if (uh->check == 0)
		{
			uh->check = -1;
		}
	}
	*/

	return 0;
}


// 
// It copies data from 'from' + offset, to 'to' for 'len' number of bytes. 
//
int
aosCopyDataToSkb(unsigned char *from, 
				 unsigned char *to, 
				 int offset, 
				 int len, 
				 struct sk_buff *skb)
{
	if (skb->ip_summed == CHECKSUM_HW) 
	{
		// 
		// Copy the data 
		// 
		memcpy(to, from + offset, len);
	} 
	else 
	{
		// 
		// Copy and checksum
		//
		skb->csum += csum_partial_copy_generic(from + offset, to, len, 0, NULL, NULL);
	}
	return 0;
}


int aosCreateUdpSkb(struct sk_buff **skb, 
					void *from, 
					int length, 
					int priority,
					struct rtable *rt,
					struct ip_options *opt,
					unsigned int saddr, 
					unsigned int daddr,
					unsigned short sport,
					unsigned short dport,
					unsigned int flags, 
					unsigned char doChecksum, 
					unsigned char identity, 
					unsigned char protocol,
					unsigned char tos)
{
	int ret;

	struct sk_buff_head skbChain;
	unsigned int udphdr = sizeof(struct udphdr);
	skb_queue_head_init(&skbChain);

	// 
	// Create the skbs first. Skbs are stored in skbChain.
	//
	if ((ret = aosCreateSkbs(from, length+udphdr, 
			udphdr, rt, opt, dport, flags, &skbChain)))
	{
		return ret;
	}

	*skb = skb_peek(&skbChain);

	// 
	// Set the UDP header
	//
	ret = aosSetUdpHdr(&skbChain, udphdr, 
		saddr, daddr, sport, dport, length, doChecksum);

	ret |= aosSetIPHdr(*skb, saddr, daddr, opt, &rt->u.dst, 
		identity, protocol, tos, priority);

	(*skb)->dev = rt->u.dst.dev;

	return ret;
}


int aosCreateTcpSkb(struct sk_buff **skb, 
					void *from, 
					int datalen, 
					int priority,
					struct rtable *rt,
					struct ip_options *opt,
					unsigned int saddr, 
					unsigned int daddr,
					unsigned short sport,
					unsigned short dport,
					unsigned int flags, 
					unsigned char doChecksum, 
					unsigned char identity, 
					unsigned char protocol,
					unsigned char tos)
{
	/*
	int ret;

	struct sk_buff_head skbChain;
	u32 tcphdr = sizeof(struct tcphdr);
	skb_queue_head_init(&skbChain);

	// 
	// Create the skbs first. Skbs are stored in skbChain.
	//
	if ((ret = aosCreateSkbs(from, datalen+tcphdr, 
			udphdr, rt, opt, dport, flags, &skbChain)))
	{
		return ret;
	}

	*skb = skb_peek(&skbChain);

	// 
	// Set the UDP header
	//
	return aosSetTcpHdr(skb, tcphdrlen, saddr, daddr, sport, dport, 
		datalen, seq, ackseq, doff, fin, ack, syn, rst, urg, ece, cwr,
		window, urgptr, doChecksum);

	ret = aosSetTcpHdr(&skbChain, udphdr, 
		saddr, daddr, sport, dport, length, doChecksum);

	ret |= aosSetIPHeader(*skb, saddr, daddr, opt, &rt->u.dst, 
		identity, protocol, tos, priority);

	(*skb)->dev = rt->u.dst.dev;
	return ret;
	*/

	return 0;
}


// 
// The skb has already reserved IP header. This function sets the header
// based on the parameters passed in. 
//
int aosSetIPHdr(struct sk_buff *skb, 
				   u32 saddr,
				   u32 daddr, 
				   struct ip_options *opt, 
				   struct dst_entry *dst,
				   int identity, 
				   int protocol,
				   int tos,
				   int priority)
{
	struct iphdr *iph;
	int df = 0;
	__u8 ttl = 64;

	if (!skb)
	{
		printk("Error: skb list is empty\n");
		return -1;
	}

	//
	// move skb->data to ip header from ext header
	//
	if (skb->data < skb->nh.raw)
	{
		// 
		// skb-data is smaller than skb->nh.raw means that skb->data is in front
		// of skb->nh. This can be the case (I guess) that there is the 'ext header'. 
		// The statement below moves 'data' pointer to where 'nh.raw' is. 
		// In other word, after the following statement, 'data' points to the network
		// header. This is what the IP layer cares about.
		// 
		__skb_pull(skb, skb->nh.raw - skb->data);
	}

	iph = (struct iphdr *)skb->data;
	iph->version = 4;
	iph->ihl = 5;
	if (opt) 
	{
		// Not implemented yet
		// iph->ihl += opt->optlen>>2;
		// ip_options_build(skb, opt, inet->cork.addr, rt, 0);
	}
	iph->tos = tos;
	iph->tot_len = htons(skb->len);
	iph->frag_off = df;
	if (!df) 
	{
		__ip_select_ident(iph, dst, 0);
	} 
	else 
	{
		iph->id = htons(identity);
	}

	iph->ttl = ttl;
	iph->protocol = protocol;
	iph->saddr = saddr;
	iph->daddr = daddr;
	ip_send_check(iph);

	skb->priority = priority;
	skb->dst = dst_clone(dst);

	return 0;
}


