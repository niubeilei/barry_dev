////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosIpOutput.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/aosIpOutput.h"

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

// #include "aos/aosKernelError.h"
#include "aosUtil/aosSkb.h"
#include <string.h>

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
	if ((ret = aosCreateSkbs(from, length+transhdrlen, 
			transhdrlen, rt, opt, dport, flags, &skbChain)))
	{
		return ret;
	}

	skb = skb_peek(&skbChain);

	// 
	// Create the UDP header
	//
	ret = aosSetUdpHdr(&skbChain, transhdrlen, 
		saddr, daddr, sport, dport, length, doChecksum);

	// 
	// Push the pending frames
	//
	ret = aosPushPendingFrames(&skbChain, rt, opt, identity, protocol, tos, priority);

	return ret;
}


