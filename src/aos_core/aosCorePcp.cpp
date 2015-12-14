////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCorePcp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosCorePcp.h"

#include "KernelSimu/netdevice.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/aosKernelMutex.h"

#include "aos/KernelEnum.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosKernelApi.h"
#include "aos/aosBridge.h"
#include "aos/aosBuiltinRules.h"
#include "aos_core/aosRule.h"
#include "aos_core/aosMacCtlr.h"
#include "aosUtil/StringUtil.h"


struct aosRule *sgAosNetInputPcpData[eAosMaxDev] = {0};
aosKernelMutex sgLock = __SEMAPHORE_INITIALIZER(sgLock, 1);
// aosKernelMutex sgLock = {0};

int	aosNetworkInputPcpStatus = 0;

int aosNetInputPcp_init(void)
{
	int i;
	for (i=0; i<eAosMaxDev; i++)
	{
		sgAosNetInputPcpData[i] = 0;
	}

	return 0;
}


/*
int aosAddNetInputPcp(struct aosRuleNetInput *rule)
{
	struct net_device *dev;

	if (rule->mDevIndex < 0 || rule->mDevIndex >= eAosMaxDev)
	{
		aosKernelAlarm(eAosAlarmIndexErr, rule->mDevIndex);
		return eAosAlarm_aosAddNetInputPcp_indexErr;
	}

	dev = aosGetDevByIndex(rule->mDevIndex);
	if (!dev)
	{
		aosKernelAlarm(eAosAlarm_aosAddNetInputPcp_devNotFound, rule->mDevIndex);
		return eAosAlarm_aosAddNetInputPcp_devNotFound;
	}

	dev_hold(dev);

	// Lock the data

	if (sgAosNetInputPcpData[rule->mDevIndex])
	{
		aosEnqueueTail(sgAosNetInputPcpData[rule->mDevIndex], rule);
	}
	else
	{
		rule->mPrev = 0;
		rule->mNext = 0;
		sgAosNetInputPcpData[rule->mDevIndex] = rule;
	}

	// unlock the data

	return 0;
}
*/


// 
// This function is called when the system receives a packet from a network
// card and is ready to process the packet. This function is not part of the
// network card interrupt function. 
//
int aosNetInputPcp_proc(struct sk_buff *skb)
{
	int ret;
	struct aosRule *rule = 0;

	if (!skb->dev)
	{
		return aosAlarmRated(eAosAlarmDeviceNull, 1, (int)skb, 0, 0, 0);
	}

	// 
	// Check the MAC Controller
	//
	if (aosMacCtlrFlag)
	{
		switch ((ret = aosMacCtlr_proc(skb)))
		{
		case eAosRc_PktConsumed:
		 	 return ret;

		case eAosRc_NoAosPath:
		 	 // 
		 	 // Should not proceed. Go back to the normal path.
		 	 // 
		 	 return 0;

		case eAosRc_ContAosPath:
		 	 break;

		default:
		 	 // 
	 	 	 // This indicates an error. Raise the alarm.
		 	 //
         	 aosAlarmRated(eAosAlarm_NetInputPcp_proc, 5, ret, 0, 0, 0);
		 	 return eAosRc_PktConsumed;
		}
	}
			
	// 
	// Check the bridging module.
	//
	if ((ret = aosBridge_proc(skb)) == eAosRc_PktConsumed)
	{
		return ret;
	}
	
	aosKernelLock(sgLock);
	// 
	// Check the IP flow array. If a rule is associated for this packet,
	// use the rule to process it. Otherwise, return eAosRc_Continue;
	//
	// rule = array[...];
	aosKernelUnlock(sgLock);

	if (!rule)
	{
		// 
		// No rule for this packet. 
		// 
		aosKernelUnlock(sgLock);
		return eAosRc_Continue;
	}

	return rule->mHead.mProc(rule, skb);
}


/*
int aosBridgeRule_create(unsigned char ruleId,
					int numDev1, 
					char names1[eAosMaxDev][eAosMaxDevNameLen], 
					int weight1[],
					int numDev2, 
					char names2[eAosMaxDev][eAosMaxDevNameLen], 
					int weight2[])
{
	struct aosBridgeRule *rule1;
	struct aosBridgeRule *rule2;
	struct aosBridgeDef *bridge1 = 0;
	struct aosBridgeDef *bridge2 = 0;
	struct aosFwdDev *fwddev = 0;
	int i, ret;

	aosKernelLock(sgLock);

	// 
	// Make sure the rule was not already defined.
	//
	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosNetInputPcpData[i] && 
			sgAosNetInputPcpData[i]->mRuleId == ruleId)
		{
			aosKernelUnlock(sgLock);
			return aosWarnInt(eAosWarnRuleExist, ruleId);
		}
	}

	// 
	// Create the bridges
	//
	bridge1 = aosBridge_constructor(ruleId, numDev1, 
		eAosBridge_Left, names1, weight1, &ret);

	if (bridge1 == NULL)
	{
		aosKernelUnlock(sgLock);
		return ret;
	}

	bridge2 = aosBridge_constructor(ruleId, numDev2, 
		eAosBridge_Right, names2, weight2, &ret);

	if (bridge2 == NULL)
	{
		aosKernelUnlock(sgLock);
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
				aosKernelUnlock(sgLock);
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
	rule1 = kmalloc(sizeof(struct aosBridgeRule), GFP_KERNEL);
	rule2 = kmalloc(sizeof(struct aosBridgeRule), GFP_KERNEL);
	if (!rule1 || !rule2)
	{
		aosKernelUnlock(sgLock);
		aosBridge_destructor(bridge1);
		aosBridge_destructor(bridge2);
		return aosAlarm(eAosAlarmMemErr);
	}
		
	memset(rule1, 0, sizeof(struct aosBridgeRule));
	memset(rule2, 0, sizeof(struct aosBridgeRule));

	rule1->mRuleId = ruleId;
	rule1->mType = eAosRuleType_BridgeRule;
	rule1->mProc = aosBridgeRule_proc;
	rule1->mBridge = bridge1;

	rule2->mRuleId = ruleId;
	rule2->mType = eAosRuleType_BridgeRule;
	rule2->mProc = aosBridgeRule_proc;
	rule2->mBridge = bridge2;

	// 
	// Install the rules
	//
	fwddev = bridge1->mDevInfo;
	while (fwddev)
	{
		int index = fwddev->mDev->ifindex;
		if (sgAosNetInputPcpData[index])
		{
			aosBridgeRule_remove(rule2);
			aosKernelUnlock(sgLock);
			kfree(rule1);
			kfree(rule2);
			return aosAlarm(eAosAlarmSlotNotEmpty, 
				sgAosNetInputPcpData[index]->mRuleId, 
				sgAosNetInputPcpData[index]->mType, 0, 0);
			
		}
		else
		{			
			sgAosNetInputPcpData[index] = (struct aosRule*)rule2;
		}

		fwddev = fwddev->mNext;
	}

	fwddev = bridge2->mDevInfo;
	while (fwddev)
	{
		int index = fwddev->mDev->ifindex;
		if (sgAosNetInputPcpData[index])
		{
			aosAlarm(eAosAlarmSlotNotEmpty, 
				sgAosNetInputPcpData[index]->mRuleId, 
				sgAosNetInputPcpData[index]->mType, 0, 0);
			aosBridgeRule_remove(rule1);
			aosBridgeRule_remove(rule2);
			aosKernelUnlock(sgLock);
			kfree(rule1);
			kfree(rule2);
			return eAosAlarmSlotNotEmpty;	
		}
		else
		{
			sgAosNetInputPcpData[index] = (struct aosRule*)rule1;
		}

		fwddev = fwddev->mNext;
	}

	aosKernelUnlock(sgLock);
	return 0;	
}
*/


int aosNetInputPcp_delRule(int ruleId)
{
	int i;
	struct aosRule *rule = 0;

	aosKernelLock(sgLock);
	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosNetInputPcpData[i] &&
			sgAosNetInputPcpData[i]->mHead.mRuleId == ruleId)
		{
			rule = sgAosNetInputPcpData[i];
			sgAosNetInputPcpData[i] = 0;
		}
	}

	aosKernelUnlock(sgLock);
	if (!rule)
	{
		return aosWarnInt(eAosWarnRuleNotFound, ruleId);
	}

	if (!rule->mHead.mDestructor)
	{
		return aosAlarmInt(eAosAlarmRuleDestructorNull, ruleId);
	}
	
	rule->mHead.mDestructor(rule);

	return 0;
}


// 
// It removes 'rule' from the array.
//
void aosNetInputPcp_remove(struct aosRule *rule, int lockflag)
{
	int i;

	if (lockflag) aosKernelLock(sgLock);

	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosNetInputPcpData[i] == rule)
		{
			sgAosNetInputPcpData[i] = 0;
		}
	}

	if (lockflag) aosKernelUnlock(sgLock);
}


struct aosRule *
aosNetInputPcp_getRule(int ruleId, int lockFlag)
{
	int i;
	struct aosRule *rule = 0;

	if (lockFlag) aosKernelLock(sgLock);

	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosNetInputPcpData[i] && 
			sgAosNetInputPcpData[i]->mHead.mRuleId == ruleId)
		{
			rule = sgAosNetInputPcpData[i];
			if (lockFlag) aosKernelUnlock(sgLock);
			return rule;
		}
	}

	if (lockFlag) aosKernelUnlock(sgLock);
	return 0;
}


void aosNetInputPcp_lock(struct aosRule ***pcpdata)
{
	aosKernelLock(sgLock);
	*pcpdata = sgAosNetInputPcpData;
}


void aosNetInputPcp_unlock(void)
{
	aosKernelUnlock(sgLock);
}


int aosCorePcp_statusCli(char *data, 
						 unsigned int *length, 
						 struct aosKernelApiParms *parms,
						 char *errmsg, 
						 const int errlen)
{
	char *pcpname = parms->mStrings[0];
	char *status = parms->mStrings[1];
	int ret = 0;
	int flag;
	if (strcmp(status, "on") == 0)
	{
		flag = 1;
	}
	else if (strcmp(status, "off") == 0)
	{
		flag = 0;
	}
	else
	{
		*length = 0;
		sprintf(errmsg, "Invalid argument: %s", status);
		return eAosRc_InvalidArg;
	}

	if (strcmp(pcpname, "netinput") == 0)
	{
		aosNetworkInputPcpStatus = flag;
	}
	else
	{
		sprintf(errmsg, "Unknown PCP: %s", pcpname);
		ret = eAosRc_UnknownPcp;
	}

	*length = 0;
	return ret;
}


int aosCorePcp_showPcpCli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms,
						  char *errmsg, 
						  const int errlen)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	char local[200];
	unsigned int optlen = *length;

	if (aosNetworkInputPcpStatus)
	{
		sprintf(local, "\nNetwork Input PCP: on\n");
	}
	else
	{
		sprintf(local, "\nNetwork Input PCP: off\n");
	}
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	if (aosMacCtlrFlag)
	{
		sprintf(local, "MAC Controller:    on\n");
	}
	else
	{
		sprintf(local, "MAC Controller:    off\n");
	}
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}

