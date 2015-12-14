////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosAfInet.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_ipv4/.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/netdevice.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/neighbour.h>
#include <KernelSimu/jiffies.h>
#include <KernelSimu/spinlock.h>

#include "aos/KernelEnum.h"
#include "aos/aosIpv4.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosDev.h"

#include <string.h>

extern struct neigh_table arp_tbl;


// 
// This function is a slight modification of 
// arp_find(unsigned char *haddr, struct sk_buff *skb).
// It finds the MAC address for the IP address: 'ipaddr'.
// 
// Parameters:
//	'haddr': 	If a MAC is found, it is returned through this variable.
//  'ipaddr':	The IP address of which the MAC is to be found.
//  'dev':		The device through which the ipaddr can be reached. 
int aosGetMac(unsigned char *haddr, 
			  unsigned int ipaddr, 
			  struct net_device *dev)
{
	struct neighbour *n;

	if (!dev) 
	{
		printk("aosArpFind is called with dev==NULL\n");
		return 1;
	}

	// 
	// If the address is the following type:
	//  * RTN_LOCAL: 
	// 	  the address is owned by this device. It will use its own 
	// 	  MAC address (from 'dev')
	//  * RTN_MULTICAST:
	// 	  the MAC address can be calculated based on the IP address (paddr)
	//  * RTN_BROADCAST:
	//    the MAC address is copied from 'dev'
	// 
	if (arp_set_predefined(inet_addr_type(ipaddr), haddr, ipaddr, dev))
	{
		return 0;
	}

	// 
	// It is none of the above. Look up the table using the IP address ipaddr.
	// Note that this should not fail unless it runs out of memory since the
	// 'create' flag is set to 1. 
	//
	n = __neigh_lookup(&arp_tbl, &ipaddr, dev, 1);

	if (n) 
	{
		// 
		// An entry is found. Update the timestamp. 
		//
		n->used = (unsigned long)jiffies;

		// 
		// If the entry state is valid or sending neigh_event is successful
		// copy the MAC address from the entry and return.
		// 
		if (n->nud_state&NUD_VALID || neigh_event_send(n, NULL) == 0) 
		{
			read_lock_bh(&n->lock);
 			memcpy(haddr, n->ha, dev->addr_len);
			read_unlock_bh(&n->lock);
			neigh_release(n);
			return 0;
		}

		// 
		// The entry was found but it is not valid. 
		//
		neigh_release(n);
	} 
	else
	{
		// 
		// 'n' should never be NULL unless it runs out of memory. When this
		// happens, all we can do is to drop the packet. 
		//
		// kfree_skb(skb);
	}

	return 1;
}


int aosGetMacByName(unsigned char *haddr, 
			   unsigned int ipaddr, 
			   const char *devname)
{
	struct net_device *dev = aosGetDevByName(devname);
	if (!dev)
	{
		return eAosErr_DeviceNotFound;
	}

	return aosGetMac(haddr, ipaddr, dev);
}


