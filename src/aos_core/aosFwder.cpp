////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosFwder.cpp
// Description:
//	A Forwarder maintains a list of recipients. When it is called, 
//  it forwards the packet to one of the recipients. A Forwarder
//  has a number of output devices and a number of receivers. It 
//  assumes all receivers can be reached through any of the devices. 
//  The purposes of using multiple devices are several:
//		* Reliability: if one device fails, another can be used.
//		* Load balance: traffic can be load balanced among all devices.
//  The seleciton of devices can be controlled by an algorithms. 
//
//  For the same reason, there are a number of receivers. When 
//  forwarding, it selects one receiver. The selection
//  of the recipients is done by an algorithm. 
// 
//  Potential algorithms for both device and receiver selection include:
//		* Round Robin
//		* Weighted Round Robin
//		* Pakect Overflow
//		* Bandwidth Overflow
//		* Primary/Standby (non-preemptive and preemptive)  
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosFwder.h"

#include <KernelSimu/netdevice.h>
#include <KernelSimu/core.h>
#include <KernelSimu/dev.h>
#include <KernelSimu/skbuff.h>

#include "aos/aosKernelAlarm.h"
#include "aos/aosDebug.h"
#include "aos/aosFwdDev.h"
#include "aos/aosSkbRecv.h"
#include "aos/aosCoreComm.h"
#include "aos/aosDev.h"
#include "aos/KernelEnum.h"

#ifdef AOS_KERNEL_SIMULATE
#include <string.h>
#endif

// Chen Ding, 03/03/2007
#if 0
// 
// Reset it to its default value.
//
int aosFwder_clear(struct aosFwder *self)
{
	struct aosFwdDev *crtdev = 0; 
	struct aosFwdDev *nextdev = 0; 
	struct aosSkbRecv *crtrecv = 0;
	struct aosSkbRecv *nextrecv = 0;
	
    self->mPrev = 0;
	self->mNext = 0;
	// self->mList = 0;
	aosPktStat_reset(&self->mStat);

	// 
	// Remove all fwddev
	//
	crtdev = self->mDevHead;
	while (crtdev)
	{
		nextdev = crtdev->mNext;
		aosFwdDev_destructor(crtdev);
		crtdev = nextdev;
	}
	self->mDevHead = 0;
	self->mNumDevs = 0;
	self->mCrtDev = 0;
	self->mCrtDevWeight = 0;
	self->mDevMethod = eAosDefaultDevMethod;
	self->mDevHit = 0;

	// 
	// Remove all receivers
	//
	crtrecv = self->mRecvHead;	
	while (crtrecv)
	{
		nextrecv = crtrecv->mNext;
		aosSkbRecv_destructor(crtrecv);
		crtrecv = nextrecv;
	}
    self->mRecvHead = 0;
    self->mCrtRecv = 0;
    self->mCrtRecvWeight = 0;
    self->mRecvMethod = eAosDefaultRecvMethod;
    self->mNumRecvs = 0;
    self->mRecvHit = 0;
    self->mUseOwnMac = 1;
			
	return 0;
}


// 
// This is the function called when a packet needs to be forwarded. If 
// the packet is forwarded, it returns 0. Otherwise, it returns an 
// error code and the skb is dropped.
//
// IMPORTANT: no matter whether the skb is forwarded or not, it is GONE!!!
// The caller must clone the skb if it wants to keep it or use it after
// calling this function.
// 
int aosFwder_forward(struct aosFwder *self, struct sk_buff *skb)
{
	int ret = 0;

	if (!self->mDevHead)
	{
		// No output device for this forwarder.
		ret = aosAlarmRated(eAosAlarmR_NoDevice, 60, 
			self->mNumDevs, self->mFwdId, 0, 0);
		goto error;
	}

	if (!self->mCrtDev)
	{
		// Should not happen, but could, if all devices are down.
		ret = aosAlarmRated(eAosAlarmR_DevNull, 60, 
			self->mNumDevs, self->mFwdId, 0, 0);
		goto error;
	}

	// 
	// Select the outbound device
	//
	switch (self->mDevMethod)
	{
	case eAosMethod_None:
		 ret = aosAlarmRated(eAosAlarmR_InvalidMethod, 
			3, self->mFwdId, self->mDevMethod, 0, 0);
		 goto error;

	case eAosMethod_WRR:
		 self->mDevHit++;
		 if (self->mNumDevs > 1 && 
			 self->mDevHit >= self->mCrtDev->mWeight)
		 {
			// 
			// Go to the next one.
			//
			self->mCrtDev = 
				(self->mCrtDev->mNext)?self->mCrtDev->mNext:self->mDevHead;
			self->mDevHit = 1;
		 }
		 break;

	case eAosMethod_RR:
		 if (self->mNumDevs > 1)
		 {
			self->mCrtDev = 
				(self->mCrtDev->mNext)?self->mCrtDev->mNext:self->mDevHead;
		 }
		 break;

	default:
		 ret = aosAlarmRated(eAosAlarmR_UnknownMethod, 3, 
			self->mFwdId, self->mDevMethod, 0, 0);
		 goto error;
	}

	// 
	// Select Receiver
	// 
	switch (self->mRecvMethod)
	{
	case eAosMethod_None:
	 	 ret = aosAlarmRated(eAosAlarmR_InvalidMethod, 5, 
			self->mFwdId, self->mRecvMethod, 0, 0);
	 	 goto error;

	case eAosMethod_WRR:
	 	 self->mRecvHit++;
	 	 if (self->mNumRecvs > 1 && 
			 self->mRecvHit >= self->mCrtRecv->mWeight)
	 	 {
			// 
			// Go to the next one.
			//
			self->mCrtRecv = (self->mCrtRecv->mNext)?
				self->mCrtRecv->mNext:self->mRecvHead;
				self->mRecvHit = 1;
	 	 }
	 	 break;

	case eAosMethod_RR:
	 	 if (self->mNumRecvs > 1)
	 	 {
			self->mCrtRecv = (self->mCrtRecv->mNext)?
				self->mCrtRecv->mNext:self->mRecvHead;
	 	 }
	 	 break;

	default:
	 	 ret = aosAlarmRated(eAosAlarmR_UnknownMethod, 5, 
			self->mFwdId, self->mRecvMethod, 0, 0);
	 	 goto error;
	}

	// 
	// If it comes to this point, we have determined the receiver 
	// and the outbound device. It is the time to send it.
	//

	// 
	// Last minute check!!!!!!
	//
	if (!self->mCrtDev || !self->mCrtDev->mDev)
	{
		ret = aosAlarmRated(eAosAlarmR_DevNull, 5, 
			self->mFwdId, (unsigned int)self->mCrtDev, 0, 0);
		goto error;
	}

	if (!self->mCrtRecv)
	{
		ret = aosAlarmRated(eAosAlarmR_NoReceiver, 5, 
			self->mFwdId, 0, 0, 0);
		goto error;
	}
	
    //
    // IMPORTANT: this function assumes the ether header has already
	// been put into the skb. Note that if the skb was just received, 
	// the driver stripped the ether header.
	//
	if (skb->mac.raw != skb->data)
	{
		if (skb->data - skb->head < ETH_HLEN)
		{
			// 
			// This is not a correct skb. Drop it
			// 
        	ret = aosAlarmRated(eAosAlarmR_ForwardedSkbNotCorrect, 5,
				(unsigned int)skb->head, (unsigned int)skb->data, 0, 0);
			goto error;
		}
    
		skb_push(skb, ETH_HLEN);
		skb->mac.raw = skb->data;
	}

	// 
	// Modify the MAC
	//
	skb->dev = self->mCrtDev->mDev;
	if (self->mUseOwnMac)
	{	
		memcpy(((struct ethhdr *)skb->mac.raw)->h_source, 
			self->mCrtDev->mDev->dev_addr, ETH_ALEN);
	}

	memcpy(((struct ethhdr *)skb->mac.raw)->h_dest, 
			self->mCrtRecv->mMac, ETH_ALEN);

	// 
	// Send it
	//
	ret = dev_queue_xmit(skb);

	//
	// Statistics. Note that 'ret == 0' does not guarantee the skb 
	// was indeed sent, but it is the best we can get for now. 
	//
	if (ret == 0)
	{
		self->mStat.mTotalPkts++;
		self->mStat.mTotalBytes += skb->len;
		self->mCrtDev->mStat.mTotalPkts++;
		self->mCrtDev->mStat.mTotalBytes += skb->len;
		self->mCrtRecv->mStat.mTotalPkts++;
		self->mCrtRecv->mStat.mTotalBytes += skb->len;
	}
	else
	{
		self->mStat.mErrorPkts++;
		self->mStat.mErrorBytes += skb->len;
		self->mCrtDev->mStat.mErrorPkts++;
		self->mCrtDev->mStat.mErrorBytes += skb->len;
		self->mCrtRecv->mStat.mErrorPkts++;
		self->mCrtRecv->mStat.mErrorBytes += skb->len;
	}

	return ret;

error:
	self->mStat.mErrorPkts++;
	self->mStat.mErrorBytes += skb->len;
    kfree_skb(skb);
	return ret;
}


struct aosFwder *
aosFwder_constructor(unsigned short flags,
					 int devMethod,
					 int recvMethod,
					 int useOwnMac,
					 int numDev, 
					 char names[eAosMaxDev][eAosMaxDevNameLen],
					 int devWeight[],
					 int numRecv, unsigned int addr[], int recvWeight[],
					 int *ret)
{
	struct aosFwder *fwder = 0;
	struct aosSkbRecv *crtrecv = 0;
	struct aosSkbRecv *prerecv = 0;
	struct aosFwdDev *crtdev = 0;
	struct aosFwdDev *predev = 0;
	struct net_device *dev = 0;
	struct net_device *devFound = 0;
	int i;

	if (numDev < 0 || numDev >= eAosMaxDev)
	{
		*ret = aosAlarmInt(eAosAlarmInvalidNumDev, numDev);
		return 0;
	}

	if (numRecv < 0 || numRecv >= eAosMaxFwderRecv)
	{
		*ret = aosAlarmInt(eAosAlarmInvalidNumRecv, numRecv);
		return 0;
	}

	fwder = (struct aosFwder*)kmalloc(sizeof(struct aosFwder), GFP_KERNEL);
	if (fwder == NULL)
	{
		*ret = aosAlarm(eAosAlarmMemErr);
		return 0;
	}

	memset(fwder, 0, sizeof(struct aosFwder));

	// 
	// Create the forward devices 
	// 
	for (i=0; i<numDev; i++)
	{
		dev = aosGetDevByName(names[i]);
		if (!dev)
		{
			aosFwder_destructor(fwder);
			*ret = aosAlarmStr(eAosAlarmDeviceNotFound, names[i]);
			return 0;
		}

		if (dev->ifindex < 0 || dev->ifindex >= eAosMaxDev)
		{
			dev_put(dev);
			*ret = aosAlarmInt(eAosAlarmIndexErr, dev->ifindex);
			aosFwder_destructor(fwder);
			return 0;
		}

		crtdev = aosFwdDev_constructor(dev, devWeight[i], ret);
		if (!crtdev)
		{
			dev_put(dev);
			aosFwder_destructor(fwder);
			return 0;
		}

		if (!devFound)
		{
			devFound = dev;
		}
		else
		{
			dev_put(dev);
		}

		if (!predev)
		{
			predev = crtdev;
			fwder->mDevHead = crtdev;

			crtdev->mNext = 0;
			crtdev->mPrev = 0;
		}
		else
		{
			predev->mNext = crtdev;
			crtdev->mNext = 0;
			crtdev->mPrev = predev;
		}
	}

	// 
	// If no device found, we can't create receivers. This is an error.
	// 
	if (!devFound)
	{
		*ret = aosAlarm(eAosAlarmNoDev);
		aosFwder_destructor(fwder);
		return 0;
	}

	// 
	// Create receivers 
	// 
	for (i=0; i<numRecv; i++)
	{
		crtrecv = aosSkbRecv_constructor(devFound, addr[i], recvWeight[i], ret);
		if (!crtrecv)
		{
			dev_put(devFound);
			aosFwder_destructor(fwder);
			return 0;
		}

		if (!prerecv)
		{
			prerecv = crtrecv;
			fwder->mRecvHead = crtrecv;

			crtrecv->mNext = 0;
			crtrecv->mPrev = 0;
		}
		else
		{
			prerecv->mNext = crtrecv;
			crtrecv->mNext = 0;
			crtrecv->mPrev = prerecv;
		}
	}

	dev_put(devFound);
	fwder->mFwdId = 0;
	fwder->mFlags = flags;
	fwder->mCrtDev = fwder->mDevHead;
	fwder->mDevMethod = devMethod;
	fwder->mNumDevs = numDev;
	fwder->mDevHit = 0;

	fwder->mCrtRecv = fwder->mRecvHead;
	fwder->mRecvMethod = recvMethod;
	fwder->mNumRecvs = numRecv;
	fwder->mRecvHit = 0;
	fwder->mUseOwnMac = useOwnMac;

	fwder->mForward = aosFwder_forward;
	*ret = 0;
	return fwder;
}


int aosFwder_destructor(struct aosFwder *fwder)
{
	struct aosFwdDev *fwddev;
	struct aosFwdDev *nextdev;
	struct aosSkbRecv *recv;
	struct aosSkbRecv *nextrecv;

	if (fwder->mPrev || fwder->mNext)
	{
		return aosAlarm4(eAosAlarmDelObjInList, 
			fwder->mFwdId, (unsigned int)fwder->mPrev, 0, 0);
	}

	fwddev = fwder->mDevHead;	
	while (fwddev)
	{
		nextdev = fwddev->mNext;
		aosFwdDev_destructor(fwddev);
		fwddev = nextdev;
	}

	recv = fwder->mRecvHead;
	while (recv)
	{
		nextrecv = recv->mNext;
		aosSkbRecv_destructor(recv);
		recv = nextrecv;
	}
		
	memset(fwder, 0, sizeof(struct aosFwder));
	kfree(fwder);
	return 0;
}

#endif

