////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: dev.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/dev.h"

#include "KernelSimu/aosKernelMutex.h"
#include "KernelSimu/netdevice.h"
#include "KernelSimu/skbuff.h"

#include "Debug/Debug.h"
#include "aos/KernelEnum.h"

#include <string.h>


static OmnMutexType		dev_base_lock;
static net_device *		sgDevList[eAosMaxDev] = {0};


int dev_init()
{

	sgDevList[0] = netdevice_create("eth0", 0);
	sgDevList[1] = netdevice_create("eth1", 1);
	sgDevList[2] = netdevice_create("eth2", 2);
	sgDevList[3] = netdevice_create("eth3", 3);
	sgDevList[4] = netdevice_create("eth4", 4);
	sgDevList[5] = netdevice_create("eth5", 5);
	sgDevList[6] = netdevice_create("eth6", 6);
	sgDevList[7] = netdevice_create("eth7", 7);

	return 0;
}


int dev_queue_xmit(struct sk_buff *skb)
{
	// 
	// We simply free the skb.
	//
	kfree_skb(skb);
	return 0;
}


struct net_device *dev_get_by_index(int index)
{
	if (index < 0 || index >= eAosMaxDev)
	{
		return 0;
	}

	return sgDevList[index];
}


struct net_device *dev_get_by_name(const char *name)
{
	struct net_device *dev;

	aosKernelLock(dev_base_lock);
	dev = __dev_get_by_name(name);
	if (dev)
	{
		dev_hold(dev);
	}
	aosKernelUnlock(dev_base_lock);
	return dev;
}


struct net_device *__dev_get_by_name(const char *name)
{
	// 
	// We maintain a list of devices in sgDevList.
	//
	for (int i=0; i<eAosMaxDev; i++)
	{
		if (sgDevList[i] && strcmp(sgDevList[i]->name, name) == 0)
		{
			return sgDevList[i];
		}
	}

	return NULL;
}


void dev_set_promiscuity(struct net_device *dev, int inc)
{
	// unsigned short old_flags = dev->flags;

	dev->flags |= IFF_PROMISC;
	/*
	if ((dev->promiscuity += inc) == 0)
		dev->flags &= ~IFF_PROMISC;
	if (dev->flags ^ old_flags) {
		dev_mc_upload(dev);
		printk(KERN_INFO "device %s %s promiscuous mode\n",
		       dev->name, (dev->flags & IFF_PROMISC) ? "entered" :
		       					       "left");
	}
	*/
}
