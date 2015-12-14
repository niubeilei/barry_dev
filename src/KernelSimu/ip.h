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

#ifndef Omn_aos_KernelSimu_ip_h
#define Omn_aos_KernelSimu_ip_h

#ifdef AOS_KERNEL_SIMULATE

#include <KernelSimu/sock.h>

#include "KernelSimu/types.h"


struct iphdr {
	__u8	ihl:4,
		version:4;

	// 
	// If it is big ending, use the lines below
	//
	// __u8	version:4,
  	// 	ihl:4;
	__u8	tos;
	__u16	tot_len;
	__u16	id;
	__u16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__u16	check;
	__u32	saddr;
	__u32	daddr;
};

struct ip_options {
  __u32		faddr;				/* Saved first hop address */
  unsigned char	optlen;
  unsigned char srr;
  unsigned char rr;
  unsigned char ts;
  unsigned char is_setbyuser:1,			/* Set by setsockopt?			*/
                is_data:1,			/* Options in __data, rather than skb	*/
                is_strictroute:1,		/* Strict source route			*/
                srr_is_hit:1,			/* Packet destination addr was our one	*/
                is_changed:1,			/* IP checksum more not valid		*/	
                rr_needaddr:1,			/* Need to record addr of outgoing dev	*/
                ts_needtime:1,			/* Need to record timestamp		*/
                ts_needaddr:1;			/* Need to record addr of outgoing dev  */
  unsigned char router_alert;
  unsigned char __pad1;
  unsigned char __pad2;
  unsigned char __data;
};

#define optlength(opt) (sizeof(struct ip_options) + opt->optlen)

struct ip_options;
struct rtable;

#include "KernelPorting/ip.h"

#else
#include <linux/ip.h>
#endif

#endif

