////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSkbRecv.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosSkbRecv.h"

#include <KernelSimu/netdevice.h>
#include <KernelSimu/skbuff.h>
#include <KernelSimu/core.h>
#include <KernelSimu/string.h>

#include "aos/aosKernelAlarm.h"
#include "aos/aosDebug.h"
#include "aos/aosArp.h"

// Chen Ding, 03/03/2007
#if 0

// 
// The constructor will hold the device. 
//
struct aosSkbRecv *
aosSkbRecv_constructor(struct net_device *dev, 
					   unsigned int addr,
					   int weight, 
					   int *ret)
{
	struct aosSkbRecv *recv = 0;
	
	recv = (struct aosSkbRecv*)kmalloc(sizeof(struct aosSkbRecv), GFP_KERNEL);
	if (!recv)
	{
		*ret = aosAlarm(eAosAlarmMemErr);
		return 0;
	}
			
	memset(recv, 0, sizeof(struct aosSkbRecv));
	dev_hold(dev);
	recv->mDev = dev;
	recv->mAddr = addr;
	recv->mWeight = weight;
	aosGetMac(recv->mMac, addr, dev);
	recv->mMacGood = (memcpy(recv->mMac, 0, ETH_ALEN) == 0)?0:1;

	*ret = 0;
	return recv;
}


int aosSkbRecv_destructor(struct aosSkbRecv *self)
{
	dev_put(self->mDev);
	memset(self, 0, sizeof(struct aosSkbRecv));
	kfree(self);
	return 0;
}

#endif

