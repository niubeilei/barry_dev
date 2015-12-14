////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosIpOutput.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosIpOutput.h"

#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/netdevice.h>
#include <linux/snmp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/udp.h>
#include <net/route.h>
#include <net/dst.h>
#include <net/ip.h>
#include "aos/aosKernelAlarm.h"

/*
 *	Combined all pending IP fragments on the socket as one IP datagram
 *	and push them out.
 */
// 
// The function treats all the skbs in the write queue belong to the same
// datagram. It will first put all skbs in the queue to the fraglst of the
// first skb. Or in other word, it combines all skbs in the write queue
// into just one. 
//
// After that, it constructs the ip header for the (master) skb. Note that
// there will be one and only one skb after the above step. 
//
// Then it calls the NF_HOOK. The subsequent function to call after the hook
// is (normally) ip_output(...), also defined in this file. 
//
int aosPushPendingFrames(struct sk_buff_head *skbs, 
						 struct rtable *rt, 
						 struct ip_options *opt, 
						 int identity, 
						 int protocol,
						 int tos,
						 int priority)
{
	struct sk_buff *skb = __skb_dequeue(skbs);
	struct sk_buff *tmp_skb;
	struct sk_buff **tail_skb;
	struct iphdr *iph;
	int df = 0;
	__u8 ttl;
	int err = 0;

	if (!skb)
	{
		printk("Error: skb list is empty\n");
		return -1;
	}

	// 
	// 'frag_list' is a list of fragmented skbs. 'tail_skb' is used
	// to construct the list. The first skb in the sk_write_queue serves
	// as the root skb. This skb's frag_list should point to the first
	// fragmented skb, and that skb's next pointer should point to the 
	// next fragmented skb, and so on. 
	//
	tail_skb = &(skb_shinfo(skb)->frag_list);

	/* move skb->data to ip header from ext header */
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

	// 
	// Combine all skbs in the write queue into one
	//
	while ((tmp_skb = __skb_dequeue(skbs)) != NULL) 
	{
		// 
		// This function assumes all the packets in the write queue belongs to the
		// same datagram. What it does in this loop is to attach all the skbs
		// in the write queue to the fraglist of the first skb. All the lengths in
		// the subsequent skbs are added to that of the first skb. But when doing
		// this, it removes the ip header since it does not belong to the data portion.
		// This is the reason why it 'pulls' skb->h.raw - skb->nh.raw. 
		//
		__skb_pull(tmp_skb, skb->h.raw - skb->nh.raw);

		// 
		// If this is the first loop, "tail_skb" is the address of 
		// 'skb_shinfo(skb)->frag_list'
		// *tail_skb = tmp_skb will make 'skb_shinfo(skb)->frag_list points to the first 
		// fragmented skb. The statement 'tail_skb = &(tmp_skb->next)' then is the address
		// of the first fragmented skb's 'next' field. This field will point to the next
		// fragmented skb, and so on. This eventually creates a list of skbs. 
		// 
		*tail_skb = tmp_skb;
		tail_skb = &(tmp_skb->next);

		// 
		// It adds 'len', 'data_len', and 'truesize'.
		//
		skb->len += tmp_skb->len;
		skb->data_len += tmp_skb->len;
		skb->truesize += tmp_skb->truesize;

		// 
		// We assume the skb is not associated with a sk
		//
		tmp_skb->destructor = NULL;
		tmp_skb->sk = NULL;
	}

	/* Unless user demanded real pmtu discovery (IP_PMTUDISC_DO), we allow
	 * to fragment the frame generated here. No matter, what transforms
	 * how transforms change size of the packet, it will come out.
	 */
	//if (inet->pmtudisc != IP_PMTUDISC_DO)
	//{
	//	printk("CD: ppp\n");
	//	skb->local_df = 1;
	//}

	/* DF bit is set when we want to see DF on outgoing frames.
	 * If local_df is set too, we still allow to fragment this frame
	 * locally. */
	//if (inet->pmtudisc == IP_PMTUDISC_DO ||
	//    (!skb_shinfo(skb)->frag_list && ip_dont_fragment(sk, &rt->u.dst)))
	//{
	//	printk("CD: pppp\n");
	//	df = htons(IP_DF);
	//}

	ttl = 64;

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
		__ip_select_ident(iph, &rt->u.dst, 0);
	} else 
	{
		iph->id = htons(identity);
	}

	iph->ttl = ttl;
	iph->protocol = protocol;
	iph->saddr = rt->rt_src;
	iph->daddr = rt->rt_dst;
	ip_send_check(iph);

	skb->priority = priority;
	skb->dst = dst_clone(&rt->u.dst);

	/* Netfilter gets whole the not fragmented skb. */
	// 
	// Call the hook. Normally 'dst_output' will call ip_output(...). 
	//
	err = NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, 
		      skb->dst->dev, dst_output);
	if (err) 
	{
		// if (err > 0)
		// {
		// 	err = inet->recverr ? net_xmit_errno(err) : 0;
		// }

		if (err)
		{
			goto error;
		}
	}

error:
	IP_INC_STATS(IPSTATS_MIB_OUTDISCARDS);
	return err;
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

int aosCreateSkbs(void *from, 
			   int length, 
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
	int mtu = 0;
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
		// contains the transhdrlen. 
		data = skb_put(skb, fraglen);

		// CD: Appears that the ext header will be in front of the standard
		// network header. What is 'ext header'?
		skb->nh.raw = data + exthdrlen;
		data += fragheaderlen;
		skb->h.raw = data + exthdrlen;

		copy = datalen - transhdrlen;
		if (copy > 0 && aosCopyDataToSkb(from, data + transhdrlen, offset, copy, skb) < 0) 
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
// 'datalen': the entire UDP datagram data length (not including the header length)
//
int aosCreateUdpHdr(struct sk_buff_head *skbs,
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
			skb->csum = csum_partial((char *)uh,
					sizeof(struct udphdr), skb->csum);

			uh->check = csum_tcpudp_magic(saddr, daddr,
					datalen + transhdrlen, IPPROTO_UDP, skb->csum);

			if (uh->check == 0)
			{
				uh->check = -1;
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
			skb->csum = csum_partial((char *)uh,
					sizeof(struct udphdr), skb->csum);
		}

		skb_queue_walk(skbs, skb) 
		{
			csum = csum_add(csum, skb->csum);
		}
		
		uh->check = csum_tcpudp_magic(saddr, daddr,
				uh->len, IPPROTO_UDP, csum);

		if (uh->check == 0)
		{
			uh->check = -1;
		}
	}

	return 0;
}


/*
 * This is a function copied from ip_output.c::ip_append_data(...). It is
 * used to create skbs for a UDP datagram. 
 * 
 * Limitations:
 */
int aosUdpSend(void *from, 
			   int length, 
			   int transhdrlen,
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

	struct sk_buff *skb;
	struct sk_buff_head skbChain;
	skb_queue_head_init(&skbChain);

	// 
	// Create the skbs first. Skbs are stored in skbChain.
	//
	if ((ret = aosCreateSkbs(from, length+transhdrlen, transhdrlen, rt, opt, dport, flags, 
					&skbChain)))
	{
		return ret;
	}

	skb = skb_peek(&skbChain);

	// 
	// Create the UDP header
	//
	ret = aosCreateUdpHdr(&skbChain, transhdrlen, 
		saddr, daddr, sport, dport, length, doChecksum);

	// 
	// Push the pending frames
	//
	ret = aosPushPendingFrames(&skbChain, rt, opt, identity, protocol, tos, priority);

	return ret;
}


