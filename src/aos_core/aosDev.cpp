////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosDev.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosDev.h"

#include <KernelSimu/netdevice.h>
#include <KernelSimu/dev.h>
#include "aos/KernelEnum.h"
#include "aos/aosKernelAlarm.h"


struct net_device * aosGetDevByName(const char *name)
{
	return dev_get_by_name(name);
}


struct net_device *aosGetDevByIndex(int index)
{
	return dev_get_by_index(index);
}
	

int aosSetDevPromiscuity(struct net_device *dev)
{
	// 
	// Note that net_device.promiscuity is a counter. This function increments
	// the counter by 1. If the counter is still <= 0, an error code is returned.
	//
	dev_set_promiscuity(dev, 1);
	if (dev->promiscuity <= 0)
	{
		return eAosErr_PromiscuityNegative;
	}

	return 0;
}


int aosSetDevPromiscuityByName(const char *name)
{
	// 
	// Note that net_device.promiscuity is a counter. This function increments
	// the counter by 1. If the counter is still <= 0, an error code is returned.
	//
	struct net_device *dev = aosGetDevByName(name);
	if (!dev)
	{
		return eAosErr_DeviceNotFound;
	}

	dev_set_promiscuity(dev, 1);
	if (dev->promiscuity <= 0)
	{
		return eAosErr_PromiscuityNegative;
	}

	return 0;
}


int aosResetDevPromiscuity(struct net_device *dev)
{
	// 
	// This function decrement the dev.promiscuity counter. If it becomes
	// 0, the device will go back to normal mode.
	//
	dev_set_promiscuity(dev, -1);
	return 0;
}


int aosResetDevPromiscuityByName(const char *name)
{
	// 
	// This function decrement the dev.promiscuity counter. If it becomes
	// 0, the device will go back to normal mode.
	//
	struct net_device *dev = aosGetDevByName(name);
	if (!dev)
	{
		return eAosErr_DeviceNotFound;
	}

	dev_set_promiscuity(dev, -1);
	if (dev->promiscuity > 0)
	{
		return eAosErr_PromiscuityPositive;
	}

	return 0;
}


int aosIsDevPromiscuity(struct net_device *dev)
{
	// 
	// This function decrement the dev.promiscuity counter. If it becomes
	// 0, the device will go back to normal mode.
	//
	return (dev->flags & IFF_PROMISC) ? 1:0;
}


int aosIsDevPromiscuityByName(const char *name, int *rslt)
{
	// 
	// This function decrement the dev.promiscuity counter. If it becomes
	// 0, the device will go back to normal mode.
	//
	struct net_device *dev = aosGetDevByName(name);
	if (!dev)
	{
		return eAosErr_DeviceNotFound;
	}

	*rslt = (dev->flags & IFF_PROMISC) ? 1:0;
	
	return 0;
}


