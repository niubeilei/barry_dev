////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosFwdDev.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosFwdDev.h"

#include <KernelSimu/netdevice.h>
#include <KernelSimu/skbuff.h>
#include <KernelSimu/core.h>
#include "aos/aosKernelAlarm.h"
#include "aos/aosDebug.h"

#ifdef AOS_KERNEL_SIMULATE
#include <string.h>
#endif


// Chen Ding, 03/03/2007
#if 0
// 
// The constructor will call dev_hold(dev). If the caller does not
// hold the device itself, it should either call dev_put(dev) to 
// release it, or it didn't hold it at all.
//
struct aosFwdDev *
aosFwdDev_constructor(struct net_device *dev, 
					  int weight, 
					  int *ret)
{
	struct aosFwdDev *fwddev;

	fwddev = (struct aosFwdDev*)kmalloc(sizeof(struct aosFwdDev), GFP_KERNEL);
	if (!fwddev)
	{
		*ret = aosAlarm(eAosAlarmMemErr);
		return 0;
	}
			
	memset(fwddev, 0, sizeof(struct aosFwdDev));
	fwddev->mDev = dev;
	dev_hold(dev);
	fwddev->mWeight = weight;

	return fwddev;
}


int aosFwdDev_destructor(struct aosFwdDev *self)
{
	dev_put(self->mDev);
	memset(self, 0, sizeof(struct aosFwdDev));
	kfree(self);
	return 0;
}


int aosFwdDev_reset(struct aosFwdDev *self)
{
	aosPktStat_reset(&self->mStat);
	return 0;
}

#endif

