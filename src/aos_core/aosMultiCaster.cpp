////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMultiCaster.cpp
// Description:
//	A MultiCaster forwards a packet to a list of recipients. When it is
//  called, it will forward the pkt to each of the recipients one by
//  one. There is no priority. If more than one recipient is defined
//  in the forwarding list, the skb is cloned for each recipient 
//  except the first one. When forwarding, each forwarder can elect
//  using the outbound MAC address or the original MAC address. Each 
//  forwarder may have multiple receivers. In that case, forwarding
//  is performed based on a load balance algorithm. 
//
//  MultiCaster defines an array:
//
//		struct aosFwder *sgAosMultiCaster[eAosMaxMultiCaster]
//
//  This array is indexed (normally) by receiving device index (but
//  it can be indexed by anything else). If we want to forward all
//  packets received from device 5, 7, and 101 to certain recipients,
//  for instance, we can define a multicaster myCaster and assign it to:
//		sgAosMultiCaster[5]   = myCaster;
//		sgAosMultiCaster[7]   = myCaster;
//		sgAosMultiCaster[101] = myCaster;
//	
//  Note that multi-casters are different from bridges in that bridges are
//  symmetric while multi-casters are asymmetric.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosMultiCaster.h"

#include <KernelSimu/aosKernelMutex.h>
#include <KernelSimu/netdevice.h>
#include <KernelSimu/skbuff.h>
#include <KernelSimu/core.h>
#include <KernelSimu/string.h>

#include "aos/KernelEnum.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosDebug.h"
#include "aos/aosFwder.h"

// Chen Ding, 03/03/2007
#if 0
static struct aosMultiCaster *sgAosMultiCaster[eAosMaxCaster] = {0};
static aosKernelMutex sgLock = __SEMAPHORE_INITIALIZER(sgLock, 1);


int 
aosMultiCaster_init(void)
{
	int i;
	for (i=0; i<eAosMaxCaster; i++)
	{
		sgAosMultiCaster[i] = 0;
	}

	return 0;
}


// 
// Destroys all forwarders in sgAosMultiCaster.
//
int aosMultiCaster_clearAll(void)
{
	int i;

	aosKernelLock(sgLock);
	for (i=0; i<eAosMaxCaster; i++)
	{
		if (sgAosMultiCaster[i])
		{
			aosMultiCaster_destructor(sgAosMultiCaster[i]);
		}
	}

	aosKernelUnlock(sgLock);
	return 0;
}


int 
aosMultiCaster_isDefined(int mcId)
{
	int i;

	aosKernelLock(sgLock);
	for (i=0; i<eAosMaxCaster; i++)
	{
		if (sgAosMultiCaster[i] && sgAosMultiCaster[i]->mMcId == mcId)
		{
			aosKernelUnlock(sgLock);
			return 1;
		}
	}

	aosKernelUnlock(sgLock);
	return 0;
}


// 
// This is the function called when a packet needs to be multicast. If 
// the packet is cast, the function returns 0. It may fail on:
//	1. Memory error: when cloning, it may run out of memory. In this case, 
//     the skb is simply dropped and the function returns eAosAlarm_memErr.
//	2. Each forwarder may return error. In that case, it returns the last 
//     forwarder error.
//
// NOTE: No matter whether the packet is forwarded or not, skb is gone. 
// Do not use it anymore. If the caller wants to keep the skb, it should
// clone the skb first and use the cloned skb to call this function.
//
int 
aosMultiCaster_forward(struct aosMultiCaster *self, struct sk_buff *skb)
{
	struct aosFwder *crt = self->mFwders;
	struct aosFwder *prev = 0;
	struct sk_buff *s = skb;
	int ret = 0; 
	int ret1 = 0;

	//
	// Process the special cases: 0, 1
	//
	switch (self->mNumFwders)
	{
	case 0:
		 kfree_skb(skb);
		 self->mStat.mErrorPkts++;
		 self->mStat.mErrorBytes += skb->len;
		 return eAosAlarmNoForwarder;

	case 1:
		 ret = self->mFwders->mForward(self->mFwders, skb);
		 goto finish;
	}

	// 
	// Process multiple forwarders.
	//
	while (crt)
	{
		if (prev)
		{
			s = skb_clone(skb, GFP_KERNEL);
			if (!s)
			{
				kfree_skb(skb);
				return aosAlarmRated(eAosAlarmR_MemErr, 1, 0, 0, 0, 0);
			}

			ret = prev->mForward(prev, s);
			if (ret)
			{
				ret1 = ret;
			}
		}
		
		prev = crt;
		crt = crt->mNext;
	}

	if (!prev)
	{
		// 
		// This should never happen!!!
		//
		return aosAlarm(eAosAlarmPrevNull);
	}

	ret = prev->mForward(prev, skb);
	ret1 = (ret)?ret:ret1;

finish:
	if (ret1)
	{
		self->mStat.mErrorPkts++;
		self->mStat.mErrorBytes += skb->len;
	}
	else
	{
		self->mStat.mTotalPkts++;
		self->mStat.mTotalBytes += skb->len;
	}
	return ret1;
}


struct aosMultiCaster * 
aosMultiCast_constructor(unsigned char mcId, 
						 unsigned short flags, 
						 int *ret)
{
	struct aosMultiCaster *caster;

	if (aosMultiCaster_isDefined(mcId))
	{
		*ret = aosAlarmInt(eAosAlarmMcDefined, mcId);
		return 0;
	}

	caster = (struct aosMultiCaster*)
		kmalloc(sizeof(struct aosMultiCaster), GFP_KERNEL);
	if (!caster)
	{
		*ret = aosAlarmInt(eAosAlarmMemErr, mcId);
		return 0;
	}

	memset(caster, 0, sizeof(struct aosMultiCaster));
	caster->mMcId = mcId;
	caster->mFlags = flags;
	return caster;
}


int 
aosMultiCaster_addFwder(struct aosMultiCaster *self, 
						unsigned short flags, 
						int devMethod,
						int recvMethod,
						int useOwnMac,
                        int numDev, 
						char names[eAosMaxDev][eAosMaxDevNameLen], 
						int devWeight[],
                        int numRecv, 
						unsigned int addr[], 
						int recvWeight[])
{
	int ret;
	struct aosFwder *prev = 0;
	struct aosFwder *crt = 0;
	struct aosFwder *fwder = aosFwder_constructor(
               	flags, devMethod, recvMethod, useOwnMac, 
				numDev, names, devWeight, 
			   	numRecv, addr, recvWeight, &ret);

	if (!fwder)
	{
		return ret;
	} 

	crt = self->mFwders;
	while (crt)
	{
		prev = crt;
		crt = crt->mNext;
	}

	if (prev)
	{
		prev->mNext = fwder;
		fwder->mNext = 0;
		fwder->mPrev = prev;
	}
	else
	{
		self->mFwders = fwder;
	}

	self->mNumFwders++;
	return 0;
}


int 
aosMultiCaster_removeFwder(struct aosMultiCaster *self, int fwderId)
{
	struct aosFwder *fwder = self->mFwders;
	while (fwder)
	{
		if (fwder->mFwdId == fwderId)
		{
			if (fwder->mPrev)
			{
				fwder->mPrev->mNext = fwder->mNext;
			}

			if (fwder->mNext)
			{
				fwder->mNext->mPrev = fwder->mPrev;
			}

			aosFwder_destructor(fwder);
			self->mNumFwders--;
			return 0;
		}

		fwder = fwder->mNext;
	}

	// 
	// Didn't find it.
	//
	return aosAlarmInt2(eAosAlarmForwarderNotDefined, self->mMcId, fwderId);
}


int 
aosMultiCaster_destructor(struct aosMultiCaster *self)
{
	struct aosFwder *fwder = self->mFwders;
	struct aosFwder *next;
	while (fwder)
	{
		next = fwder->mNext;
		aosFwder_destructor(fwder);
		fwder = next;
	}

	memset(self, 0, sizeof(struct aosMultiCaster));
	kfree(self);
	return 0;
}

#endif

