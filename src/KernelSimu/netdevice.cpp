////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: netdevice.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/netdevice.h"

#include "aos/aosKernelAlarm.h"
#include <KernelSimu/if_ether.h>

#include <string.h>


net_device *
netdevice_create(const char *name, int index)
{
	net_device *dev = new net_device();
	if (!dev)
	{
		aosAlarm(eAosAlarmMemErr);
		return 0;
	}

	memset(dev, 0, sizeof(net_device));
	strcpy(dev->name, name);
	OmnInitMutex(dev->ingress_lock);
	OmnInitMutex(dev->xmit_lock);
	OmnInitMutex(dev->queue_lock);
	OmnInitMutex(dev->refcntLock);
	dev->ifindex = index;
	dev->hard_header_len = ETH_HLEN;
	return dev;
}

