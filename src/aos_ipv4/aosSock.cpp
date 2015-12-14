////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSock.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosPkt.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/ip.h>
#include <KernelSimu/udp.h>
#include <KernelSimu/route.h>
#include <KernelSimu/dev.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/flow.h>

#include "aos/aosIpOutput.h"
#include "aos/aosKernelAlarm.h"

#include <string.h>


// 
// This is an array of skb's for testing purpose only
//
#define OmnTestSkbArraySize 101

static struct sk_buff *sgTestSkb[OmnTestSkbArraySize];


void aosTestSkbInit()
{
	int i;

	for (i=0; i<OmnTestSkbArraySize; i++)
	{
		sgTestSkb[i] = 0;
	}
}



int aosSendSkb(struct OmnKernelApi_sendPkt *data)
{
	// struct rtable *rt = NULL;
	unsigned int length = data->mSize;
	unsigned int   saddr = data->mSendAddr;
	unsigned int   daddr = data->mRecvAddr;
	unsigned short sport = data->mSendPort;
	unsigned short dport = data->mRecvPort;
	unsigned char  tos   = data->mTos;
	unsigned char from[1010];
	struct net_device *dev = dev_get_by_name(data->mDeviceName);

	length = 1010;
	if (!dev)
	{
		return eAosErr_DeviceNotFound;
	}

	memset(from, 'c', length);	
	printk("Found device: %d\n", dev->ifindex);	
	printk("daddr=%d, saddr=%d, sport=%d, dport=%d, tos=%d\n", 
		daddr, saddr, sport, dport, tos);

/*
	// 
	// To look up the route entry
	//
	{
		struct flowi fl = { .oif = dev->ifindex,
				    		.nl_u = { .ip4_u =
					      				{ .daddr = daddr,
										  .saddr = saddr,
										  .tos = tos 
										} 
								},
				    		.proto = IPPROTO_UDP,
				    		.uli_u = { .ports =
					       				{ .sport = sport,
						 				  .dport = dport 
								 		} 
									 } 
					  };

		if (ip_route_output_flow(&rt, &fl, NULL, 0) || rt == NULL)
		{
			printk("CD: Failed to retrieve the route\n");
			return eAosErr_NoRoute;
		}

		printk("Found route\n");
	}

	aosUdpSend(from, length, sizeof(struct udphdr), 0, rt, NULL, 
		saddr, daddr, sport, dport, 0, 1, 99+1, 17, 0);
*/
	return 0;
}


/*
int aos_skb_put(struct OmnKernelApi_pktTest *data)
{
	int index = data->mIndex;
	unsigned int length = data->mSize;
	if (index < 0 || index >= OmnTestSkbArraySize)
	{
		printk("Invalid skb index: %d\n", index);
		return eAosErr_InvalidSkbIndex;
	}

	if (!sgTestSkb[index])
	{
		printk("Test skb is null: %d\n", index);
		return eAosErr_NullPointer;
	}

	data->mSize = (unsigned int)skb_put(sgTestSkb[index], length);

	return 0;
}


int aos_skb_tailroom(struct OmnKernelApi_pktTest *data)
{
	int index = data->mIndex;
	if (index < 0 || index >= OmnTestSkbArraySize)
	{
		printk("Invalid skb index: %d\n", index);
		return eAosErr_InvalidSkbIndex;
	}

	if (!sgTestSkb[index])
	{
		printk("Test skb is null: %d\n", index);
		return eAosErr_NullPointer;
	}

	data->mSize = skb_tailroom(sgTestSkb[index]);

	return 0;
}


int aos_skb_reserve(struct OmnKernelApi_pktTest *data)
{
	int index = data->mIndex;
	unsigned int length = data->mSize;
	if (index < 0 || index >= OmnTestSkbArraySize)
	{
		printk("Invalid skb index: %d\n", index);
		return eAosErr_InvalidSkbIndex;
	}

	if (!sgTestSkb[index])
	{
		printk("Test skb is null: %d\n", index);
		return eAosErr_NullPointer;
	}

	skb_reserve(sgTestSkb[index], length);

	return 0;
}


int aos_skb_push(struct OmnKernelApi_pktTest *data)
{
	int index = data->mIndex;
	unsigned int length = data->mSize;
	if (index < 0 || index >= OmnTestSkbArraySize)
	{
		printk("Invalid skb index: %d\n", index);
		return eAosErr_InvalidSkbIndex;
	}

	if (!sgTestSkb[index])
	{
		printk("Test skb is null: %d\n", index);
		return eAosErr_NullPointer;
	}

	data->mSize = (unsigned int)skb_push(sgTestSkb[index], length);

	return 0;
}


int aos_skb_pull(struct OmnKernelApi_pktTest *data)
{
	int index = data->mIndex;
	unsigned int length = data->mSize;
	if (index < 0 || index >= OmnTestSkbArraySize)
	{
		printk("Invalid skb index: %d\n", index);
		return eAosErr_InvalidSkbIndex;
	}

	if (!sgTestSkb[index])
	{
		printk("Test skb is null: %d\n", index);
		return eAosErr_NullPointer;
	}

	data->mSize = (unsigned int)skb_pull(sgTestSkb[index], length);

	return 0;
}


void aosShowTestSkb(struct OmnKernelApi_pktTest *data)
{
	int index = data->mIndex;
	if (index < 0 || index >= OmnTestSkbArraySize)
	{
		printk("Invalid skb index: %d\n", index);
		return;
	}

	if (!sgTestSkb[index])
	{
		printk("Test skb is null: %d\n", index);
		return;
	}

	aosShowSkb(sgTestSkb[index]);
	return;
}


void aosShowSkb(struct sk_buff *skb)
{
	struct skb_shared_info *shinfo = skb_shinfo(skb);
	printk("next=%d,   prev=%d,   list=%d,   sk=%d,   dev=%d,   input_dev=%d,   real_dev=%d\n", 
		    (unsigned int)skb->next, 
			(unsigned int)skb->prev, 
			(unsigned int)skb->list, 
			(unsigned int)skb->sk, 
			(unsigned int)skb->dev, 
			(unsigned int)skb->input_dev, 
			(unsigned int)skb->real_dev);

	printk("dst=%d, len=%d, data_len=%d, mac_len=%d, csum=%d, local_df=%d\n",
	 	 	(unsigned int)skb->dst, 
			skb->len, 
			skb->data_len, 
			skb->mac_len, 
			skb->csum, 
			skb->local_df);

	printk("h.raw=%d,      nh.raw=%d,       mac.raw=%d\n", 
			(unsigned int)(skb->h).raw, 
			(unsigned int)(skb->nh).raw, 
		    (unsigned int)(skb->mac).raw);

	printk("cloned=%d, ip_summed=%d, priority=%d, security=%d\n", 
			skb->cloned, 
			skb->ip_summed, 
			skb->priority, 
			skb->security);

	printk("destructor=%d, truesize=%d, head=%d, data=%d, tail=%d, end=%d\n", 
			(unsigned int)skb->destructor, 
			skb->truesize, 
			(unsigned int)skb->head, 
			(unsigned int)skb->data, 
			(unsigned int)skb->tail, 
			(unsigned int)skb->end);

	printk("skb size=%d, skb start=%d\n", 
			sizeof(struct sk_buff), 
			(unsigned int)skb);

	if (shinfo)
	{
		printk("nr_frags=%d, frag_list=%d\n", 
			shinfo->nr_frags, 
			(unsigned int)shinfo->frag_list);
	}
	else
	{
		printk("shinfo=0\n");
	}
}
*/
