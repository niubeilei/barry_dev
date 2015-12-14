////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMacCtlr.cpp
// Description:
//	This file defines the MAC Controller module. MAC Controller 
//	provides Layer 2 packet processing, such as filtering, manipulations,
//  etc. 
//
//	The module uses a six-dimensional array to provide fast lookups. 
//  This is the array aosMacCtlData[][][][][][]. It is indexed on MAC
//  addresses.
//
//	MAC Controller is turned on/off by the global variable 'aosMacCtlrFlag'.
//  By default, it is turned off. 
//
//  There are three built-in rules for MAC Controller:
//	Rule 1: Filter: it filters packets by MAC addresses
//  Rule 2: No AOS: it blocks AOS from processing the packets
//  Rule 3: MAC AOS: it indicates the packet should be processed by
//			MAC Controller.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosMacCtlr.h"

#include <KernelSimu/skbuff.h>
#include <KernelSimu/if_ether.h>

#include "aos/aosCoreComm.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosKernelApi.h"
#include "aos_core/aosMacRule.h"
#include "aos_core/aosRule.h"
#include "aos_core/aosMacRule.h"
#include "KernelUtil/TArray.h"
#include "aosUtil/StringUtil.h"
#include "KernelUtil/KernelMem.h"
#include "aosUtil/Slab.h"


struct 	aosMacRule *	******aosMacCtlData = 0;
int 	aosMacCtlrFlag = 0;
static 	struct aosPktStat	sgMacStat;
static	struct aosMacRule   sgMacRuleList;
static  int					sgNumRules = 0;



int aosMacCtlr_init(void)
{
	int ret;
	aosMacCtlrFlag = 0;
	aosPktStat_reset(&sgMacStat);

	ret = OmnTArray_init((void ***)&aosMacCtlData, 6, 0);

	sgMacRuleList.mHead.mPrev = (struct aosRule *)&sgMacRuleList;
	sgMacRuleList.mHead.mNext = (struct aosRule *)&sgMacRuleList;
	sgNumRules = 0;

	ret |= aosMacRule_init();
	return ret;
}

// 
// This is the main entrance point of MAC Controller. It is called
// when a packet is received and before it is passed on to the registered.
//
// It will return one of the following: 
//	eAosRc_PktConsumed:		The packet has been consumed by this funciton.
//							No one should use it.
//	eAosRc_NoAosPath:		No further AOS processing. The caller should
//							continue the normal stack processing. 
//	eAosRc_ContAosPath:		The caller should continue the AOS processing.
//
// IMPORTANT!!!!!!!!!! It should never return anything but the above
// three values. 
//
int aosMacCtlr_proc(struct sk_buff *skb)
{
    //  
    // It is an arp packet. Ask the MAC address to determine whether
    // we should bridge it or not.
    //
	unsigned char *mac;
	struct aosMacRule *rule;
    if (skb->data - skb->head < ETH_HLEN)
    {
        // 
        // Should never happen. Drop the packet.
        //
        sgMacStat.mErrorPkts++;
        sgMacStat.mErrorBytes += skb->len;
        kfree_skb(skb);
        aosAlarmRated(eAosAlarm_MacCtlr_proc, 1,
                (unsigned int)skb->head, (unsigned int)skb->data, 0, 0);
        return eAosRc_PktConsumed;
    }
      
    mac = (unsigned char *)skb->mac.raw;
    rule = aosMacCtlData[mac[0]][mac[1]][mac[2]][mac[3]][mac[4]][mac[5]];
    if (!rule)
    {
        //
        // No rule is configured for this MAC. Continue the AOS processing.
		//
		return eAosRc_ContAosPath;
	}

	// 
	// Ask the rule to process it.
	// 
	return rule->mHead.mProc((struct aosRule *)rule, skb);
}


int aosMacCtlr_macFilterRule(struct aosRule *self, struct sk_buff *skb)
{
	// 
	// This an action to filter the skb. 
	// 
	kfree_skb(skb);
	return eAosRc_PktConsumed;
}


int aosMacCtlr_macNoAosRule(struct aosRule *self, struct sk_buff *skb)
{
	// 
	// This rule returns the skb to the normal packet processing route.
	// In other word, there shall be no aos processing at MAC layer. 
	//
	return eAosRc_NoAosPath;
}


int aosMacCtlr_procMacRule(struct aosRule *self, struct sk_buff *skb)
{
	// 
	// This rule marks the skb to be processed by AOS MAC Controller.
	// 
	return eAosRc_Continue;
}


int aosMacCtlr_statusCli(char *data, unsigned int *length)
{
	char *status;
	unsigned int optlen = *length;

	int ret = aosKernelApi_getStr1(1, &status, data, optlen);
	if (ret)
	{
		return ret;
	}

	if (strcmp("on", status) == 0)
	{
		aosMacCtlrFlag = 1;
	}
	else if (strcmp("off", status) == 0)
	{
		aosMacCtlrFlag = 0;
	}
	else
	{
		*length = 0;
		return eAosRc_InvalidArg;
	}

	*length = 0;
	return 0;
}


int aosMacCtlr_showCli(char *data, unsigned int *length)
{
	char local[200];
	int ret;
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIdx = 0;
	unsigned int optlen = *length;
	char name[101];
	struct aosMacRule *rule;

	if (aosMacCtlrFlag)
	{
		sprintf(local, "MAC Controller: on\n");
	}
	else
	{
		sprintf(local, "MAC Controller: off\n");
	}
	ret = aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, local, strlen(local));

	sprintf(local, "Total Mac Rules: %d\n\n", sgNumRules);
	ret = aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, local, strlen(local));

	sprintf(local, "MAC                 Type\n");
	ret |= aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, local, strlen(local));
	sprintf(local, "------------------------------\n");
	ret |= aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, local, strlen(local));

	rule = (struct aosMacRule *)sgMacRuleList.mHead.mNext;
	while (rule != &sgMacRuleList && rsltIdx < optlen)
	{
		aosRule_getRuleName(rule->mHead.mType, name);
		sprintf(local, "%2x:%2x:%2x:%2x:%2x:%2x   %s\n", 
			rule->mMac[0], rule->mMac[1], rule->mMac[2], 
			rule->mMac[3], rule->mMac[4], rule->mMac[5], name);
		ret |= aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, local, strlen(local));
	
		rule = (struct aosMacRule *)rule->mHead.mNext;
	}

	*length = rsltIdx;
	return ret;
}


int aosMacCtlr_addCli(char *data, unsigned int *length)
{
	char *ascmac, *ruletype;
	unsigned char mac[10];
	unsigned int optlen = *length;
	struct aosMacRule *rule = 0;
	int type;
	struct aosRule *last;

	int ret = aosKernelApi_getStr2(1, &ascmac, &ruletype, data, optlen);
	if (ret)
	{
		return ret;
	}

	*length = 0;

	// 
	// mac is an ascii representation of the MAC:
	// mac = hhhhhhhhhhhh, where h=[0-9, a, b, c, d].
	//
	ret = aosAsciiToBinary(ascmac, (char *)mac, 6);
	if (ret)
	{
		return eAosRc_InvalidMac;
	}

	if (strcmp(ruletype, "filter") == 0)
	{
		type = eAosMacCtlrRuleType_Filter;
		rule = aosMacRule_constructor(aosMacCtlr_macFilterRule, type, 0);
	}
	else if (strcmp(ruletype, "noaos") == 0)
	{
		type = eAosMacCtlrRuleType_NoAos;
		rule = aosMacRule_constructor(aosMacCtlr_macNoAosRule, type, 0);
	}
	else if (strcmp(ruletype, "macaos") == 0)
	{
		type = eAosMacCtlrRuleType_MacAos;
		rule = aosMacRule_constructor(aosMacCtlr_procMacRule, type, 0);
	}
	else
	{
		return aosAlarm(eAosAlarm_aosMacCntl_addCli3);
	}

	if (!rule)
	{
		return aosAlarm(eAosAlarm_aosMacCtlr_addCli1);
	}

	aosMacCtlData[mac[0]][mac[1]][mac[2]][mac[3]][mac[4]][mac[5]] = rule;

	// 
	// Add the entry to the list
	//
	memcpy(rule->mMac, mac, 6);
	rule->mHead.mType = type;
	last = sgMacRuleList.mHead.mPrev;
	last->mHead.mNext = (struct aosRule *)rule;
	rule->mHead.mPrev = last;
	rule->mHead.mNext = (struct aosRule *)&sgMacRuleList;
	sgMacRuleList.mHead.mPrev = (struct aosRule *)rule;
	sgNumRules++;
	return 0;
}


int aosMacCtlr_delCli(char *data, unsigned int *length)
{
	char *ascmac;
	unsigned char mac[10];
	unsigned int optlen = *length;
	struct aosMacRule *rule = 0;

	int ret = aosKernelApi_getStr1(1, &ascmac, data, optlen);
	if (ret)
	{
		return ret;
	}

	*length = 0;

	// 
	// mac is an ascii representation of the MAC:
	// mac = hhhhhhhhhhhh, where h=[0-9, a, b, c, d].
	//
	ret = aosAsciiToBinary(ascmac, (char *)mac, 6);
	if (ret)
	{
		return eAosRc_InvalidMac;
	}

	rule = aosMacCtlData[mac[0]][mac[1]][mac[2]][mac[3]][mac[4]][mac[5]];
	if (!rule)
	{
		return eAosRc_NoRule;
	}

	aosMacCtlData[mac[0]][mac[1]][mac[2]][mac[3]][mac[4]][mac[5]] = 0;

	(rule->mHead.mNext)->mHead.mPrev = rule->mHead.mPrev;
	(rule->mHead.mPrev)->mHead.mNext = rule->mHead.mNext;
	sgNumRules--;
	return 0;
}
