////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosBuiltinRules.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosBuiltinRules.h"

#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/netdevice.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/if_ether.h"
#include "KernelSimu/core.h"

#include "aos/KernelEnum.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosBridge.h"
#include "aos/aosCorePcp.h"
#include "aos_core/aosRule.h"

#ifdef AOS_KERNEL_SIMULATE
#include <string.h>
#endif

#if 0
// 
// This rule can be installed in the Network Input PCP. For every packet
// received, it checks whether the packet should be bridged. If yes, it will
// forward the packet through the specified output device. 
// 	1. If the destination MAC is on this machine, do not bridge it. 
// 	2. If it is a broadcast ARP request, clone the packet; bridge the
//     the cloned packet, not the original one. 
//  3. Otherwise, always bridge
//
int aosBridgeRule_proc(struct aosRule *self, struct sk_buff *skb)
{
	// 
	// 
	int ifindex;
	int cloned = 0;
	struct aosBridgeRule *bridgeRule = 0;
	struct ethhdr *eth = (struct ethhdr *)skb->mac.raw;
	struct sk_buff *bskb = skb;

	// 
	// Make sure we ever need to worry about the bridging
	//
	if (aosBridgeStatus == eAosBridgeStatus_Inactive ||
	    memcmp(eth->h_dest, skb->dev->dev_addr, ETH_ALEN) == 0)
	{
		// Do not bridge it or it is for this machine
		return 0;
	}

	if (self->mHead.mType != eAosRuleType_BridgeRule)
	{
		aosAlarmRated(eAosAlarmR_RuleMismatch, 3, 
			self->mHead.mRuleId, self->mHead.mType, 0, 0);
		return 0;
	}

	// 
	// The skb must set 'eth'. Otherwise, we don't know its MACs. 
	// If a packet is received from a NIC, the driver should 
	// set skb->mac.raw. If skb->mac.raw is null, the packet is
	// not bridged.
	//
	if (!eth)
	{
		aosAlarmRated(eAosAlarmR_EthNull, 1, skb->len, 0, 0, 0);
		return 0;
	}

	// If it is broadcast, clone it 
	if (memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN) == 0)
	{
		bskb= skb_clone(skb, GFP_KERNEL);
		if (!bskb)
		{
			aosAlarmRated(eAosAlarmMemErr, 1, 0, 0, 0, 0);
			return 0;
		}

		cloned = 1;
	}

	// 
	// Otherwise, it is not for this machine. Need to bridge it.
	//
	ifindex = bskb->dev->ifindex;
	if (ifindex < 0 || ifindex >= eAosMaxDev)
	{
		// Should never happen
		if (cloned)
		{
			kfree_skb(bskb);
		}

		aosAlarmRated(eAosAlarmInvalidDeviceIndex, 1, 
			ifindex, 0, skb->dev->name, 0);
		return 0;
	}

	bridgeRule = (struct aosBridgeRule*)self;
	if (!bridgeRule->mBridge)
	{
		aosAlarmRated(eAosAlarmR_BridgeNull, 1, self->mHead.mRuleId, 0, 0, 0);
		if (cloned)
		{
			kfree_skb(bskb);
		}
		return 0;
	}

	bridgeRule->mBridge->mForward(bridgeRule->mBridge, bskb);

	if (cloned)
	{
		return 0;
	}

	return eAosRc_PktConsumed;
}


int aosBridgeRule_destructor(struct aosRule *self)
{
	if (self->mHead.mType != eAosRuleType_BridgeRule)
	{
		return aosAlarm4(eAosAlarmInvalidRuleTypeInDest, 
			self->mHead.mRuleId, self->mHead.mType, 0, 0);
	}

	aosNetInputPcp_remove((struct aosRule *)self, 1);
	aosBridge_destructor(((struct aosBridgeRule*)self)->mBridge);
	memset(self, 0, sizeof(struct aosBridgeRule));
	kfree(self);
	return 0;
}


/*
int aosBridgeRule_create(unsigned char ruleId,
					char *names1, 
					int weight1,
					char *names2,
					int weight2)
{
	struct aosBridgeRule *rule1;
	struct aosBridgeRule *rule2;
	struct aosBridgeDef *bridge1 = 0;
	struct aosBridgeDef *bridge2 = 0;
	struct aosFwdDev *fwddev = 0;
	int i, ret;
	struct aosRule **pcpdata;

	aosNetInputPcp_lock(&pcpdata);

	// 
	// Make sure the rule was not already defined.
	//
	for (i=0; i<eAosMaxDev; i++)
	{
		if (pcpdata[i] && 
			pcpdata[i]->mRuleId == ruleId)
		{
			aosNetInputPcp_unlock();
			return aosWarnInt(eAosWarnRuleExist, ruleId);
		}
	}

	// 
	// Create the bridges
	//
	bridge1 = aosBridge_constructor(ruleId, 
		eAosBridge_Left, names1, weight1, &ret);

	if (bridge1 == NULL || ret != 0)
	{
		aosNetInputPcp_unlock();
		return ret;
	}

	bridge2 = aosBridge_constructor(ruleId, 
		eAosBridge_Right, names2, weight2, &ret);

	if (bridge2 == NULL || ret != 0)
	{
		aosNetInputPcp_unlock();
		aosBridge_destructor(bridge1);
		return ret;
	}

	// 
	// Make sure devices in names1 and names2 do not overlap
	//
	fwddev = bridge1->mDevInfo;
	while (fwddev)
	{
		struct aosFwdDev *tmp = bridge2->mDevInfo;
		while (tmp)
		{
			if (fwddev->mDev == tmp->mDev)
			{
				// 
				// Overlapped. It is an error
				//
				aosNetInputPcp_unlock();
				aosBridge_destructor(bridge1);
				aosBridge_destructor(bridge2);
				return aosAlarmInt(eAosAlarmDeviceOverlap, tmp->mDev->ifindex);
			}

			tmp = tmp->mNext;
		}

		fwddev = fwddev->mNext;
	}

	bridge1->mPeer = bridge2;
	bridge2->mPeer = bridge1;

	// 
	// Create the rules
	//
	rule1 = (struct aosBridgeRule *)kmalloc(sizeof(struct aosBridgeRule), GFP_KERNEL);
	rule2 = (struct aosBridgeRule *)kmalloc(sizeof(struct aosBridgeRule), GFP_KERNEL);
	if (!rule1 || !rule2)
	{
		aosNetInputPcp_unlock();
		aosBridge_destructor(bridge1);
		aosBridge_destructor(bridge2);
		return aosAlarm(eAosAlarmMemErr);
	}
		
	memset(rule1, 0, sizeof(struct aosBridgeRule));
	memset(rule2, 0, sizeof(struct aosBridgeRule));

	rule1->mRuleId = ruleId;
	rule1->mType = eAosRuleType_BridgeRule;
	rule1->mProc = aosBridgeRule_proc;
	rule1->mDestructor = aosBridgeRule_destructor;
	rule1->mBridge = bridge1;

	rule2->mRuleId = ruleId;
	rule2->mType = eAosRuleType_BridgeRule;
	rule2->mProc = aosBridgeRule_proc;
	rule2->mDestructor = aosBridgeRule_destructor;
	rule2->mBridge = bridge2;

	// 
	// Install the rules
	//
printk("Continue 5\n");
	fwddev = bridge1->mDevInfo;
	while (fwddev)
	{
		int index = fwddev->mDev->ifindex;
printk("Continue 51\n");
		if (pcpdata[index])
		{
			aosNetInputPcp_remove((struct aosRule*)rule2, 0);
			aosNetInputPcp_unlock();
			kfree(rule1);
			kfree(rule2);
			return aosAlarm4(eAosAlarmSlotNotEmpty, 
				pcpdata[index]->mRuleId, 
				pcpdata[index]->mType, 0, 0);
			
		}
		else
		{			
			pcpdata[index] = (struct aosRule*)rule2;
		}

		fwddev = fwddev->mNext;
	}

printk("Continue 6\n");
	fwddev = bridge2->mDevInfo;
	while (fwddev)
	{
		int index = fwddev->mDev->ifindex;
printk("Continue 7\n");
		if (pcpdata[index])
		{
			aosAlarm4(eAosAlarmSlotNotEmpty, 
				pcpdata[index]->mRuleId, 
				pcpdata[index]->mType, 0, 0);
			aosNetInputPcp_remove((struct aosRule*)rule1, 0);
			aosNetInputPcp_remove((struct aosRule*)rule2, 0);
			aosNetInputPcp_unlock();
			kfree(rule1);
			kfree(rule2);
			return eAosAlarmSlotNotEmpty;	
		}
		else
		{
			pcpdata[index] = (struct aosRule*)rule1;
		}

		fwddev = fwddev->mNext;
	}

	aosNetInputPcp_unlock();
printk("Continue 8\n");
	return 0;	
}
*/

#endif

