////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosBridge.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosBridge.h"

#include "KernelSimu/netdevice.h"
#include "KernelSimu/dev.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/aosKernelMutex.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/if_ether.h"
#include "KernelSimu/gfp.h"
#include "KernelSimu/core.h"

#include "aos/KernelEnum.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosDebug.h"
#include "aos/aosDev.h"
#include "aos/aosMultiCaster.h"
#include "aos/aosKernelApi.h"
#include "aos_core/aosRule.h"
#include "aosUtil/StringUtil.h"

struct aosBridgeDef *sgAosBridgeData[eAosMaxDev] = {0};
aosKernelMutex aosBridgeLock = __SEMAPHORE_INITIALIZER(aosBridgeLock, 1);
int	   aosBridgeStatus = 0;
struct aosRule *	****aosDestCtlData;

#if 0
int aosBridge_init(void)
{
	int i;
	for (i=0; i<eAosMaxDev; i++)
	{
		sgAosBridgeData[i] = 0;
	}

	return 0;
}


#define aosBridgeCopyCli(x) \
		if (10101 - index <= strlen((x))-1) \
		{ \
			aosAlarmInt(eAosAlarmBuffTooShort, 10101); \
			return -1; \
		} \
		sprintf(&data->mStr[index], "%s\n", x); \
		index += strlen(x);


int aosBridge_saveConfigCli(char *data, 
							unsigned int *length, 
							struct aosKernelApiParms *parms, 
							char *errmsg, 
							const int errlen)
{
	// 
	// It creates the following commands
	//
	// bridge clear all
	// bridge on/off
	// bridge create <bridgeId>
	// bridge add device <bridgeId> <flag> <devicename> <weight>
	// ...
	// The next bridge
	// ...
	// 
	int i, j;
	int numBridges = 0;
	int bridgesFound[eAosMaxDev];
	int ret = 0;
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIdx = 0;
	unsigned int optlen = *length;

	aosKernelLock(aosBridgeLock);

	aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, "bridge clear all\n", 
		strlen("bridge clear all"));
	if (aosBridgeStatus)
	{
		aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, "bridge on\n",
			strlen("bridge on"));
	}
	else
	{
		aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, "bridge off\n",
			strlen("bridge off"));
	}
		
	// 
	// Create the commands for all bridges
	//
	for (i=0; i<eAosMaxDev && rsltIdx < optlen; i++)
	{
		if (sgAosBridgeData[i] && sgAosBridgeData[i]->mFlags & eAosBridge_Left)
		{
			// 
			// Check whether the bridge has been saved 
			//
			for (j=0; j<numBridges; j++)
			{
				if (bridgesFound[j] == sgAosBridgeData[i]->mBridgeId)
				{
					break;
				}
			}

			if (j < numBridges)
			{
				// 
				// The bridge was saved. Ignore 
				//
				continue;
			}

			if (numBridges >= eAosMaxDev)
			{
				aosAlarmInt(eAosAlarmTooManyBridges, numBridges);
				continue;
			}
			bridgesFound[numBridges++] = sgAosBridgeData[i]->mBridgeId;

			aosBridge_saveBridge(sgAosBridgeData[i], rsltBuff, &rsltIdx, optlen);
		}
	}

	aosKernelUnlock(aosBridgeLock);

	ret = aosBridge_healthcheck(rsltBuff, &rsltIdx, optlen);

	*length = rsltIdx;
	return 0;
}


int aosBridge_resetCli(char *data, 
					   unsigned int *length, 
					   struct aosKernelApiParms *parms,
					   char *errmsg, 
					   const int errlen)
{
	struct aosBridgeDef *bridge = 0;
	int bridgeId = parms->mIntegers[0];
	int ret; 

	*length = 0;

	aosKernelLock(aosBridgeLock);
	bridge = aosBridge_get(bridgeId, eAosBridge_Left);
	if ((ret = aosBridge_resetBridge(bridge)))
	{
		aosKernelUnlock(aosBridgeLock);
		return ret;
	}	

	bridge = bridge->mPeer;
	return aosBridge_resetBridge(bridge);
}

		
int aosBridge_resetBridge(struct aosBridgeDef *bridge)
{
	struct aosFwdDev *fwddev;

	if (!bridge)
	{
		return eAosRc_BridgeNotFound;
	}

	bridge->mStat.mTotalPkts = 0;
	bridge->mStat.mTotalBytes = 0;
	bridge->mStat.mErrorPkts = 0;
	bridge->mStat.mErrorBytes = 0;

	fwddev = bridge->mDevInfo;
	while (fwddev)
	{
		aosFwdDev_reset(fwddev);
		fwddev = fwddev->mNext;
	}

	return 0;
}


int aosBridge_clearAllCli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms, 
						  char *errmsg, 
						  const int errlen)
{
	int ret = aosBridge_clearAll(1, errmsg, errlen);
	*length = 0;
	return ret;
}


// 
// Destroys all bridges in sgAosBridge.
//
int aosBridge_clearAll(const int lock, char *errmsg, const int errlen)
{
	int i, j;
	struct aosBridgeDef *bridge1 = 0;
	struct aosBridgeDef *bridge2 = 0;

	if (lock)
	{
		aosKernelLock(aosBridgeLock);
	}

	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i])
		{
			bridge1 = sgAosBridgeData[i];
			bridge2 = bridge1->mPeer;

			if (bridge1 == bridge2)
			{
				//
				// Should never happen!!!
				//
				aosAlarmInt(eAosAlarmPeerErr, bridge1->mBridgeId);
				bridge2 = 0;
			}

			// 
			// Remove the bridge from the array
			//
			for (j=0; j<eAosMaxDev; j++)
			{
				if (sgAosBridgeData[j] && 
					sgAosBridgeData[j]->mBridgeId == bridge1->mBridgeId)
				{
					sgAosBridgeData[j] = 0;
				}
			}

			aosBridge_destructor(bridge1);
			if (bridge2)
			{
				aosBridge_destructor(bridge2);
			}
			bridge1 = 0;
			bridge2 = 0;
		}
	}

	if (lock)
	{
		aosKernelUnlock(aosBridgeLock);
	}
	return 0;
}


int aosBridge_isDefined(int bridgeId)
{
	int i;

	aosKernelLock(aosBridgeLock);
	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i] && sgAosBridgeData[i]->mBridgeId == bridgeId)
		{
			aosKernelUnlock(aosBridgeLock);
			return 1;
		}
	}

	aosKernelUnlock(aosBridgeLock);
	return 0;
}


int aosBridge_create(unsigned char bridgeId,
					 char *method,
					 char *names1, 
					 int weight1,
					 char *names2,
					 int weight2)
{
	struct aosBridgeDef *bridge1 = 0;
	struct aosBridgeDef *bridge2 = 0;
	struct aosFwdDev *fwddev = 0;
	int i, ret;

	aosKernelLock(aosBridgeLock);
	// 
	// Make sure the bridge was not already defined.
	//
	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i] && sgAosBridgeData[i]->mBridgeId == bridgeId)
		{
			aosKernelUnlock(aosBridgeLock);
			return eAosWarnBridgeExist;
		}
	}

	bridge1 = aosBridge_constructor(bridgeId,
		eAosBridge_Left, method, names1, weight1, &ret);
	if (bridge1 == NULL || ret != 0)
	{
		aosKernelUnlock(aosBridgeLock);
		return ret;
	}

	bridge2 = aosBridge_constructor(bridgeId, 
		eAosBridge_Right, method, names2, weight2, &ret);
	if (bridge2 == NULL || ret != 0)
	{
		aosKernelUnlock(aosBridgeLock);
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
				
				aosKernelUnlock(aosBridgeLock);
				
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

	sgAosBridgeData[bridge1->mDevInfo->mDev->ifindex] = bridge1;
	sgAosBridgeData[bridge2->mDevInfo->mDev->ifindex] = bridge2;

	aosKernelUnlock(aosBridgeLock);
	return 0;	
}


struct aosBridgeDef *
aosBridge_constructor(unsigned char bridgeId, 
					  unsigned short flags,
					  char *method,
					  char *name, 
					  int weight,
					  int *ret)
{
	struct aosBridgeDef *bridge = 0;
	struct aosFwdDev *fwddev = 0;
	struct aosFwdDev *crtdev = 0;
	struct net_device *dev = 0;

	*ret = 0;

	if (strcmp(method, "wrr") != 0 &&
		strcmp(method, "ipflow_wrr") != 0)
	{
		*ret = eAosRc_InvalidMethod;
		return 0;
	}

	bridge = (struct aosBridgeDef*)kmalloc(
		sizeof(struct aosBridgeDef), GFP_KERNEL);
	if (bridge == NULL)
	{
		aosAlarmInt(eAosAlarmMemErr, 0);
		*ret = eAosAlarmMemErr;
		return 0;
	}

	memset(bridge, 0, sizeof(struct aosBridgeDef));
	bridge->mBridgeId = bridgeId;
	bridge->mFlags = flags;

	dev = aosGetDevByName(name);
	if (!dev)
	{
		aosAlarm4(eAosWarnDeviceNotFound, bridgeId, 0, name, 0);

		aosBridge_destructor(bridge);
		*ret = eAosWarnDeviceNotFound;
		return 0;
	}

	if (dev->ifindex < 0 || dev->ifindex >= eAosMaxDev)
	{
		dev_put(dev);
		aosAlarmInt(eAosAlarmIndexErr, dev->ifindex);
		*ret =  eAosAlarmIndexErr;
		aosBridge_destructor(bridge);
		return 0;
	}

	fwddev = (struct aosFwdDev*)kmalloc(sizeof(struct aosFwdDev), GFP_KERNEL);
	if (!fwddev)
	{
		dev_put(dev);
		aosAlarmInt(eAosAlarmMemErr, 0);
		*ret = eAosAlarmMemErr;
		aosBridge_destructor(bridge);
		return 0;
	}
			
	fwddev->mDev = dev;
	aosSetDevPromiscuity(dev);
	fwddev->mWeight = weight;
	fwddev->mStat.mTotalPkts = 0;
	fwddev->mStat.mTotalBytes = 0;
	fwddev->mStat.mErrorPkts = 0;
	fwddev->mStat.mErrorBytes = 0;

	if (!crtdev)
	{
		crtdev = fwddev;
		bridge->mDevInfo = crtdev;

		crtdev->mNext = 0;
		crtdev->mPrev = 0;
	}
	else
	{
		crtdev->mNext = fwddev;
		fwddev->mNext = 0;
		fwddev->mPrev = crtdev;
	}

	bridge->mBridgeId = bridgeId;
	bridge->mCrtHit = 0;
	bridge->mNumDevs = 1;
	bridge->mCrtDev = bridge->mDevInfo;
	if (bridge->mDevInfo)
	{
		bridge->mCrtWeight = bridge->mDevInfo->mWeight;
	}
	else
	{
		bridge->mCrtWeight = 0;
	}

	if (strcmp(method, "wrr") == 0)
	{
		bridge->mForward = aosBridge_procWrr;
	}
	else if (strcmp(method, "ipflow_wrr") == 0)
	{
		 bridge->mForward = aosBridge_procWrrIpf;
	}
	else
	{
		 *ret = eAosRc_InvalidMethod;
		 return 0;
	}

//	aosExitFunc(eAosBridgeConstructor, 5, 0);
	return bridge;
}


int 
aosBridge_clearAllDev(struct aosBridgeDef *bridge)
{
	struct aosFwdDev *fwddev = bridge->mDevInfo;
	struct aosFwdDev *nextdev = 0;

	while (fwddev)
	{
		dev_put(fwddev->mDev);
		aosResetDevPromiscuity(fwddev->mDev);
		nextdev = fwddev->mNext;
		kfree(fwddev);
		fwddev = nextdev;
	}

	bridge->mDevInfo = 0;
	bridge->mNumDevs = 0;
	bridge->mCrtDev = 0;
	bridge->mCrtHit = 0;
	bridge->mCrtWeight = 0;
	return 0;
}


int 
aosBridge_destructor(struct aosBridgeDef *bridge)
{
	aosBridge_clearAllDev(bridge);
	if (bridge->mCaster)
	{
		// 
		// Caster not null. Possible memory leak.
		//
		aosAlarm4(eAosAlarmProgErr, bridge->mBridgeId, 0, "Caster not null", 0);
	}

	memset(bridge, 0, sizeof(struct aosBridgeDef));
	kfree(bridge);
	return 0;
}


int aosBridge_remove(int bridgeId)
{
	struct aosBridgeDef *bridge = 0;
	struct aosBridgeDef *bridge2 = 0;
	int i;

	aosKernelLock(aosBridgeLock);
	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i] && sgAosBridgeData[i]->mBridgeId == bridgeId)
		{
			bridge = sgAosBridgeData[i];
			sgAosBridgeData[i] = 0;
		}
	}
	aosKernelUnlock(aosBridgeLock);

	if (!bridge)
	{
		return eAosAlarmBridgeNotFound;
	}

	bridge2 = bridge->mPeer;	
	aosBridge_destructor(bridge);
	aosBridge_destructor(bridge2);
	return 0;
}


// 
// The caller should lock the array. 
//
struct aosBridgeDef *
aosBridge_get(int bridgeId, int flags)
{
	struct aosBridgeDef *bridge = 0;
	int i;

	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i] && sgAosBridgeData[i]->mBridgeId == bridgeId &&
			(sgAosBridgeData[i]->mFlags & flags))
		{
			bridge = sgAosBridgeData[i];
			break;
		}
	}
	
	return bridge;
}


int aosBridge_list(char *data, 
				   unsigned int *length, 
				   char *errmsg, 
				   const int errlen)
{
	int numIds = 0;
	int ids[eAosMaxDev];
	int ret = 0;
	int first = 1;
	char buf[1010];
	char side[10];
	int i, j;
	struct aosBridgeDef *bridge;
	struct net_device *dev;
	char *str = &data[4];
	unsigned int rsltIdx = 0;
	unsigned int optlen = *length;

	if (aosBridgeStatus)
	{
		sprintf(buf, "\nBridge Status: on\n");
		aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
	}
	else
	{
		sprintf(buf, "\nBridge Status: off\n");
		aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
	}

	aosKernelLock(aosBridgeLock);
	sprintf(buf, "Device Name    Bridge Name   Side\n");
	aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
	sprintf(buf, "---------------------------------\n");
	aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));

	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i])
		{
			dev = aosGetDevByIndex(i);
			if (dev)
			{
				if (sgAosBridgeData[i]->mFlags & eAosBridge_Left)
				{
					strcpy(side, "left");
				}
				else
				{
					strcpy(side, "right");
				}
	
				sprintf(buf, "%-13s  %-13d %-6s\n", 
						dev->name, sgAosBridgeData[i]->mBridgeId, side);
				aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
			}
		}
	}

	for (i=0; i<eAosMaxDev; i++)
	{
		if (sgAosBridgeData[i])
		{
			for (j=0; j<numIds; j++)
			{
				if (ids[j] == sgAosBridgeData[i]->mBridgeId)
				{
					break;
				}
			}

			if (j >= numIds)
			{
				bridge = sgAosBridgeData[i];
				if (bridge->mFlags & eAosBridge_Left)
				{
					strcpy(side, "left");
				}
				else
				{
					strcpy(side, "right");
				}

				if (first)
				{
					sprintf(buf, "\nBridge Name    Device        Side   Weight\n");
					aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
					sprintf(buf, "------------------------------------------\n");
					aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
					first = 0;
				}

				if (numIds>= eAosMaxDev)
				{
					aosKernelUnlock(aosBridgeLock);
					*length = rsltIdx;
					return aosAlarm(eAosAlarmTooManyBridges);
				}

				ids[numIds++] = bridge->mBridgeId;

				if (!bridge->mDevInfo)
				{
					sprintf(buf, "%-12d\n", bridge->mBridgeId);
					ret = aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
				}
				else
				{
					struct aosFwdDev *dev = bridge->mDevInfo;
					sprintf(buf, "%-12d   %-14s%-6s %d\n", 
						bridge->mBridgeId, 
						dev->mDev->name, side,
						dev->mWeight);
					ret = aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));

					dev = dev->mNext;
					while (dev)
					{
						sprintf(buf, "               %-14s%-6s %d\n", 
							dev->mDev->name, side,
							dev->mWeight);
						aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
						dev = dev->mNext;
					}

					bridge = bridge->mPeer;
					if (!bridge)
					{
						aosAlarm(eAosAlarm);
						continue;
					}

					if (bridge->mFlags & eAosBridge_Left)
					{
						strcpy(side, "left");
					}
					else
					{
						strcpy(side, "right");
					}
					dev = bridge->mDevInfo;
					while (dev)
					{
						sprintf(buf, "               %-14s%-6s %d\n", 
							dev->mDev->name, side,
							dev->mWeight);
						aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
						dev = dev->mNext;
					}
				}
			}
		}
	}

	aosKernelUnlock(aosBridgeLock);
	
	if (numIds <= 0)
	{
		// 
		// There are no bridges
		//
		sprintf(buf, "No bridges found\n");
		ret = aosCheckAndCopy(str, &rsltIdx, optlen, buf, strlen(buf));
	}

	ret |= aosBridge_healthcheck(str, &rsltIdx, optlen);
	*length = rsltIdx;

	return ret;
}


int aosBridge_show1(struct aosBridgeDef *bridge, 
					unsigned int *index, 
					char *result, 
					unsigned int length) 
{
	char buf[1010];
	int ret = 0;
	struct aosFwdDev *fwddev = 0;

	if (*index < 0 || *index >= length)
	{
		return aosAlarmInt2(eAosAlarmProgErr, length, *index);
	}

	if (bridge->mNumDevs)
	{
		fwddev = bridge->mDevInfo;	
		while (fwddev)
		{
			sprintf(buf, "             %-8s %-6d %-10d %-10d %-10d %-10d\n",
				fwddev->mDev->name, 
				fwddev->mWeight,
				(int)fwddev->mStat.mTotalPkts, 
				(int)fwddev->mStat.mTotalBytes,
				(int)fwddev->mStat.mErrorPkts,
				(int)fwddev->mStat.mErrorBytes);
			ret = aosCheckAndCopy(result, index, length, buf, strlen(buf));
			fwddev = fwddev->mNext;
		}
	}

	return ret;
}


int aosBridge_showCli(char *data, 
					  unsigned int *length, 
					  struct aosKernelApiParms *parms, 
					  char *errmsg, 
					  const int errlen)
{
	int numArgs = parms->mIntegers[0];
	if (numArgs == 0)
	{
		return aosBridge_list(data, length, errmsg, errlen);
	}

	return aosBridge_show(data, length, parms, errmsg, errlen);
}


int aosBridge_show(char *data, 
				   unsigned int *length, 
				   struct aosKernelApiParms *parms, 
				   char *errmsg, 
				   const int errlen)
{
	// 
	// Device Name  Bridge Name  Side
	// ------------------------------
	// ...
	//
	// Bridge Name  Device   Weight Packets    Bytes      Err Pkts   Err Bytes
	// -----------------------------------------------------------------------
	// 10
    //              eth0     20     0          0          0          0         
    //              eth1     30     0          0          0          0    
	// 

	struct aosBridgeDef *bridge = 0;
	char buf[1010];
	int ret;
	int bridgeId = parms->mIntegers[0];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;

	aosKernelLock(aosBridgeLock);

 	bridge = aosBridge_get(bridgeId, eAosBridge_Left);
	if (!bridge)
	{
		aosKernelUnlock(aosBridgeLock);
		sprintf(buf, "Bridge not found!\n");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, buf, strlen(buf));
		*length = rsltIndex;
		return eAosErr_BridgeNotFound;
	}
	
	sprintf(buf, "\nBridge Name  Device   Weight "
		"Packets    Bytes      Err Pkts   Err Bytes\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, buf, strlen(buf));
	sprintf(buf, "-----------------------------------------------------------------------\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, buf, strlen(buf));

	sprintf(buf, "%d\n", bridge->mBridgeId);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, buf, strlen(buf));

	aosBridge_show1(bridge, &rsltIndex, rsltBuff, optlen);

	bridge = bridge->mPeer;
	if (!bridge)
	{
		aosKernelUnlock(aosBridgeLock);
		sprintf(buf, "***** Peer is null. Please contact support!!!\n");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, buf, strlen(buf));
		*length = rsltIndex;
		return aosAlarmInt(eAosAlarmProgErr, bridgeId);
	}

	ret = aosBridge_show1(bridge, &rsltIndex, rsltBuff, optlen);
	aosKernelUnlock(aosBridgeLock);

	ret |= aosBridge_healthcheck(rsltBuff, &rsltIndex, optlen);
	*length = rsltIndex;
	return ret;
}


int aosBridge_addDev(int bridgeId, 
					const char *name,
					int weight, 
					const char *side)
{
	struct aosBridgeDef *bridge = 0;
	struct aosBridgeDef *peer = 0;
	struct aosFwdDev *fwddev = 0;
	struct aosFwdDev *prev, *crt;
	struct net_device *dev;
	int sideFlag;

	dev = aosGetDevByName(name);
	if (!dev)
	{
		return aosAlarmStr(eAosAlarmDeviceNotFound, name);
	}

	aosKernelLock(aosBridgeLock);
	if (dev->ifindex < 0 || dev->ifindex >= eAosMaxDev)
	{
		aosKernelUnlock(aosBridgeLock);
		return eAosAlarmInvalidDeviceIndex;
	}

	if (strcmp(side, "left") == 0)
	{
		sideFlag = eAosBridge_Left;
	}
	else if (strcmp(side, "right") == 0)
	{
		sideFlag = eAosBridge_Right;
	}
	else
	{
		return eAosRc_IncorrectSide;
	}

	// 
	// Check whether the position has been occupied
	//
	if (sgAosBridgeData[dev->ifindex])
	{
		aosKernelUnlock(aosBridgeLock);
		return eAosAlarmDeviceInBridge;
	}
		
	bridge = aosBridge_get(bridgeId, sideFlag);
	if (bridge == NULL)
	{
		aosKernelUnlock(aosBridgeLock);
		return aosAlarmInt2(eAosAlarmBridgeNotFound, bridgeId, sideFlag);
	}

	// 
	// Check whether the device is already in the bridge
	//
	fwddev = bridge->mDevInfo;
	while (fwddev)
	{
		if (fwddev->mDev == dev)
		{
			aosKernelUnlock(aosBridgeLock);
			dev_put(dev);
			return aosAlarmInt(eAosAlarmDeviceInBridge, dev->ifindex);
		}

		fwddev = fwddev->mNext;
	}

	peer = bridge->mPeer;
	if (!peer)
	{
		// 
		// Should never happen
		//
		aosKernelUnlock(aosBridgeLock);
		dev_put(dev);
		return aosAlarmInt(eAosAlarmPeerNull, bridgeId);
	}

	// 
	// Check whether the device is in peer
	//
	fwddev = peer->mDevInfo;
	while (fwddev)
	{
		if (fwddev->mDev == dev)
		{
			aosKernelUnlock(aosBridgeLock);
			dev_put(dev);
			return aosAlarmInt(eAosAlarmDeviceInPeer, bridgeId);
		}

		fwddev = fwddev->mNext;
	}

	// 
	// Ready to create
	//
	fwddev = (struct aosFwdDev*)kmalloc(sizeof(struct aosFwdDev), GFP_KERNEL);
	if (fwddev == NULL)
	{
		aosKernelUnlock(aosBridgeLock);
		dev_put(dev);
		return aosAlarmInt(eAosAlarmMemErr, 0);
	}

	// 
	// Append to the end
	//
	prev = 0;
	crt = bridge->mDevInfo;
	while (crt)
	{
		prev = crt;
		crt = crt->mNext;
	}

	fwddev->mNext = 0;
	fwddev->mPrev = prev;
	if (prev)
	{
		prev->mNext = fwddev;
	}
	else
	{
		bridge->mDevInfo = fwddev;
	}
	bridge->mNumDevs++;

	fwddev->mDev = dev;
	fwddev->mWeight = weight;
	fwddev->mStat.mTotalPkts = 0;
	fwddev->mStat.mTotalBytes = 0;
	fwddev->mStat.mErrorPkts = 0;
	fwddev->mStat.mErrorBytes = 0;

	// 
	// Need to set the device to be in the promiscuous mode.
	//
	aosSetDevPromiscuity(dev);

	// 
	// Install the rule in the position
	//
	sgAosBridgeData[dev->ifindex] = bridge;
	aosKernelUnlock(aosBridgeLock);
	return 0;
}


int aosBridge_delDev(int bridgeId, 
					char * side, 
					const char *name)
{
	struct aosBridgeDef *bridge = 0;
	struct aosFwdDev *fwddev = 0;
	struct net_device *dev = 0;
	int i;
	int sideFlag;

	dev = aosGetDevByName(name);
	if (!dev)
	{
		aosAlarmStr(eAosAlarmDeviceNotFound, name);
		return eAosAlarmDeviceNotFound;
	}

	if (strcmp(side, "left") == 0)
	{
		sideFlag = eAosBridge_Left;
	}
	else if (strcmp(side, "right") == 0)
	{
		sideFlag = eAosBridge_Right;
	}
	else
	{
		return eAosRc_IncorrectSide;
	}

	aosKernelLock(aosBridgeLock);

	bridge = aosBridge_get(bridgeId, sideFlag);
	if (bridge == NULL)
	{
		dev_put(dev);
		aosKernelUnlock(aosBridgeLock);
		return aosAlarm4(eAosAlarmBridgeNotFound, bridgeId, sideFlag, name, 0);
	}

	fwddev = bridge->mDevInfo;
	while (fwddev)
	{
		if (fwddev->mDev == dev)
		{
			if (bridge->mNumDevs <= 0)
			{
				aosWarnInt2(eAosAlarmNoDev, bridgeId, bridge->mNumDevs);
				aosKernelUnlock(aosBridgeLock);
				return eAosAlarmNoDev;
			}

			// 
			// Found the device. Remove it.
			//
			if (fwddev->mPrev)
			{
				fwddev->mPrev->mNext = fwddev->mNext;
			}

			if (fwddev->mNext)
			{
				fwddev->mNext->mPrev = fwddev->mPrev;
			}

			if (fwddev == bridge->mDevInfo)
			{
				bridge->mDevInfo = fwddev->mNext;
			}
		
			if (bridge->mCrtDev == fwddev)
			{
				bridge->mCrtDev = bridge->mDevInfo;
			}

			memset(fwddev, 1, sizeof(struct aosFwdDev));
			kfree(fwddev);


			bridge->mNumDevs--;
			// 
			// Remove the bridge from the array
			//
			if (dev->ifindex < 0 || dev->ifindex >= eAosMaxDev)
			{
				// 
				// Should never happen
				// 
				aosAlarmInt(eAosAlarm, dev->ifindex);
			}
			else
			{
				sgAosBridgeData[dev->ifindex] = 0;
			}

			// 
			// Check whether the bridge should be deleted
			//
			for (i=0; i<eAosMaxDev; i++)
			{
				if (sgAosBridgeData[i] && 
					sgAosBridgeData[i]->mBridgeId == bridge->mBridgeId)
				{
					break;
				}
			}

			if (i >= eAosMaxDev)
			{
				// 
				// Need to delete the bridge
				//
				aosBridge_destructor(bridge->mPeer);
				aosBridge_destructor(bridge);
			}	
				
			aosKernelUnlock(aosBridgeLock);
			aosResetDevPromiscuity(dev);
			dev_put(dev);
			dev_put(dev);
			return 0;
		}

		fwddev = fwddev->mNext;
	}

	// 
	// The device is not in the bridge
	//
	aosKernelUnlock(aosBridgeLock);
//	bridge->mFlags = flags;
	dev_put(dev);
	return eAosAlarmDeviceNotInBridge;
}


int aosBridge_statusCli(char *data, 
						unsigned int *length, 
					    struct aosKernelApiParms *parms, 
						char *errmsg, 
						const int errlen)
{
	char *flag = parms->mStrings[0];
	int ret = 0;
	
	*length = 0;

	if (strcmp(flag, "on") == 0)
	{
		aosBridge_enableBridging();
	}
	else if (strcmp(flag, "off") == 0)
	{
		aosBridge_disableBridging();
	}
	else
	{
		ret = eAosRc_InvalidArg;
	}

	*length = 0;
	return ret;
}


void aosBridge_enableBridging(void)
{
	aosBridgeStatus = eAosBridgeStatus_Active;
}


void aosBridge_disableBridging(void)
{
	aosBridgeStatus = eAosBridgeStatus_Inactive;
}


void aosBridge_isBridgingOn(int *value)
{
	*value = aosBridgeStatus;
}


int aosBridge_removeCli(char *data, 
						unsigned int *length, 
					    struct aosKernelApiParms *parms, 
						char *errmsg, 
						const int errlen)
{
	int bridgeId = parms->mIntegers[0];
	int ret;
	unsigned int rsltIdx = 0;
	char *rsltBuff;

	ret = aosBridge_remove(bridgeId);
	rsltBuff = aosKernelApi_getBuff(data);
	ret |= aosBridge_healthcheck(rsltBuff, &rsltIdx, *length);
	*length = rsltIdx;
	return ret;
}


int aosBridge_createKapiCli(char *data, 
							unsigned int *length, 
						    struct aosKernelApiParms *parms, 
							char *errmsg, 
							const int errlen)
{
	// 
	// This is the function called from userland to create a bridge.
	//
	int ret;
	char *method = parms->mStrings[0];
	char *name1 = parms->mStrings[1];
	char *name2 = parms->mStrings[2];
	int bridgeId = parms->mIntegers[0];
	int weight1 = parms->mIntegers[1];
	int weight2 = parms->mIntegers[2];
	unsigned int index = 0;
	char *str = &data[4];
	unsigned int optlen = *length;

	if (strlen(name1) >= eAosMaxDevNameLen ||
		strlen(name2) >= eAosMaxDevNameLen)
	{
		*length = 0;
		sprintf(errmsg, "Names too long");
		return aosAlarm(eAosAlarmDevNameTooLong);
	}

	ret = aosBridge_create(bridgeId, method, name1, weight1, name2, weight2);

	ret |= aosBridge_healthcheck(str, &index, optlen-4);

	*length = index;
	return ret;
}


int aosBridge_saveBridge(struct aosBridgeDef *bridge, 
						 char *rsltBuff, 
						 unsigned int *rsltIdx,
						 unsigned int optlen) 
{
	// 
	// The bridge must be the left side. 
	//
	struct aosFwdDev *fwddev;
	char local[200];

	if (!bridge)
	{
		return eAosErr_NullPtr;
	}

	sprintf(local, "bridge add %d %s %d %s %d\n", 
		bridge->mBridgeId, 
		bridge->mDevInfo->mDev->name,
		bridge->mDevInfo->mWeight,
		bridge->mPeer->mDevInfo->mDev->name,
		bridge->mPeer->mDevInfo->mWeight);
	aosCheckAndCopy(rsltBuff, rsltIdx, optlen, local, strlen(local));
			
	fwddev = bridge->mDevInfo->mNext;
	while (fwddev && *rsltIdx < optlen)
	{
		sprintf(local, "bridge device add %d left %s %d\n", 
			bridge->mBridgeId, 
			fwddev->mDev->name, 
			fwddev->mWeight); 
		aosCheckAndCopy(rsltBuff, rsltIdx, optlen, local, strlen(local));
		fwddev = fwddev->mNext;
	}

	fwddev = bridge->mPeer->mDevInfo->mNext;
	while (fwddev && *rsltIdx < optlen)
	{
		sprintf(local, "bridge device add %d right %s %d\n", 
			bridge->mBridgeId, 
			fwddev->mDev->name, 
			fwddev->mWeight); 
		aosCheckAndCopy(rsltBuff, rsltIdx, optlen, local, strlen(local));
		fwddev = fwddev->mNext;
	}

	return 0;
}


int aosBridge_healthcheckCli(char *data, 
							 unsigned int *length, 
						     struct aosKernelApiParms *parms, 
							 char *errmsg, 
							 const int errlen)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIdx = 0;
	unsigned int optlen = *length;
	int ret = aosBridge_healthcheck(rsltBuff, &rsltIdx, optlen);
	*length = rsltIdx;
	return ret;
}


int aosBridge_healthcheck(char *buffer, 
						  unsigned int *index, 
						  const unsigned int buflen)
{
	// 
	// This function checks the bridge data for problems. If it is
	// in production, this function should not be called. It is
	// normally used for debugging.
	// 
	int i, j;
	int numBridges = 0;
	int bridgesFound[eAosMaxDev];
	int ret = 0;
	struct aosBridgeDef *bridge;
	char local[200];

	aosKernelLock(aosBridgeLock);
	for (i=0; i<eAosMaxDev && *index < buflen; i++)
	{
		bridge = sgAosBridgeData[i];
		if (bridge)
		{
			if (bridge->mDevInfo && bridge->mDevInfo->mDev &&
				bridge->mDevInfo->mDev->ifindex >= 0 &&
				bridge->mDevInfo->mDev->ifindex < eAosMaxDev)
			{
				struct aosFwdDev *fdev = bridge->mDevInfo;
				int ss = 0;
				int found = 0;
				while (fdev && ss++ < eAosMaxDev+5)
				{
					if (fdev->mDev->ifindex == i)
					{
						found = 1;
						break;
					}

					fdev = fdev->mNext;
				}

				if (!found)
				{
					sprintf(local, "Rule installed incorrect: %d, %s, %d, %d, %d\n", 
						bridge->mBridgeId, 
						bridge->mDevInfo->mDev->name, 
						bridge->mDevInfo->mDev->ifindex, i, 
						bridge->mFlags); 
					aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
					ret = eAosRc_BridgeHcErr;
				}
			}
		}

		if (bridge && bridge->mFlags & eAosBridge_Left)
		{
			// 
			// Check whether the bridge has been saved 
			//
			for (j=0; j<numBridges; j++)
			{
				if (bridgesFound[j] == bridge->mBridgeId)
				{
					break;
				}
			}

			if (j < numBridges)
			{
				// 
				// The bridge was saved. Ignore 
				//
				continue;
			}

			if (numBridges >= eAosMaxDev)
			{
				aosAlarmInt(eAosAlarmTooManyBridges, numBridges);
				continue;
			}
			bridgesFound[numBridges++] = bridge->mBridgeId;

			ret |= aosBridge_hcOnBridge(bridge, buffer, index, buflen);
		}
	}

	aosKernelUnlock(aosBridgeLock);
	return 0;
}


int aosBridge_hcOnBridge(struct aosBridgeDef *bridge, 
						 char *buffer, 
						 unsigned int *index, 
						 const unsigned int buflen)
{
	char local[200];
	int ret = 0;

	ret = aosBridge_hcOnOneSide(bridge, buffer, index, buflen);

	if (!bridge->mPeer)
	{
		sprintf(local, "Peer is null: %d\n", bridge->mBridgeId);
		aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
		ret = eAosRc_BridgeHcErr;
	}
	else
	{
		ret |= aosBridge_hcOnOneSide(bridge->mPeer, buffer, index, buflen);
	}

	return ret;
}
	

int
aosBridge_hcOnOneSide(struct aosBridgeDef *bridge, 
					  char *buffer, 
					  unsigned int *index, 
					  const unsigned int buflen) 
{
	char local[200];
	int numDevs;
	int step;
	struct aosFwdDev *fwddev, *prev;
	int ret = 0;

	if (!bridge->mDevInfo)
	{
		sprintf(local, "DevInfo is null: %d\n", bridge->mBridgeId);
		aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
		ret = eAosRc_BridgeHcErr;
	}

	if (bridge->mFlags & ~(eAosBridge_Left | eAosBridge_Right))
	{
		sprintf(local, "Flag incorrect: %d: %d\n", 
			bridge->mBridgeId, bridge->mFlags);
		aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
		ret = eAosRc_BridgeHcErr;
	}
			
	if (!bridge->mForward)
	{
		sprintf(local, "Forwarder is null: %d\n", bridge->mBridgeId);
		aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
		ret = eAosRc_BridgeHcErr;
	}

	numDevs = 0;
	prev = 0;
	fwddev = bridge->mDevInfo;
	step = 0;
	while (fwddev && step++ < eAosMaxDev + 5)
	{
		if (!fwddev->mDev)
		{
			sprintf(local, "Dev is null: %d\n", bridge->mBridgeId);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			ret = eAosRc_BridgeHcErr;
		}

		if (fwddev->mDev->ifindex < 0 || fwddev->mDev->ifindex >= eAosMaxDev)
		{
			sprintf(local, "Dev index out bound: %d, %s, %d\n", 
				bridge->mBridgeId, 
				fwddev->mDev->name, 
				fwddev->mDev->ifindex);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			ret = eAosRc_BridgeHcErr;
		}
			
		if (!sgAosBridgeData[fwddev->mDev->ifindex] ||
			sgAosBridgeData[fwddev->mDev->ifindex]->mBridgeId != 
				bridge->mBridgeId)
		{
			sprintf(local, "Rule not installed correctly: %d, %s, %d. Corrected\n", 
				bridge->mBridgeId, 
				fwddev->mDev->name, 
				fwddev->mDev->ifindex);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			sgAosBridgeData[fwddev->mDev->ifindex] = bridge;
			ret = eAosRc_BridgeHcErr;
		}

		if (fwddev->mPrev != prev)
		{
			sprintf(local, "Prev pointer incorrect: %d. Corrected\n", 
				bridge->mBridgeId);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			fwddev->mPrev = prev;
			ret = eAosRc_BridgeHcErr;
		}

		if (fwddev->mWeight < 0)
		{
			sprintf(local, "Device weight is negative: %d. Weight = %d\n", 
				bridge->mBridgeId, fwddev->mWeight);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			ret = eAosRc_BridgeHcErr;
		}

		if (fwddev->mStat.mTotalPkts < 0 ||
			fwddev->mStat.mTotalBytes < 0 ||
			fwddev->mStat.mErrorPkts < 0 ||
			fwddev->mStat.mErrorBytes < 0)
		{
			sprintf(local, "Stat incorrect: %d. Stat = %d, %d, %d, %d\n", 
				bridge->mBridgeId, 
				(int)fwddev->mStat.mTotalPkts, 
				(int)fwddev->mStat.mTotalBytes, 
				(int)fwddev->mStat.mErrorPkts, 
				(int)fwddev->mStat.mErrorBytes);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			ret = eAosRc_BridgeHcErr;
		}

		if (bridge->mStat.mTotalPkts < 0 ||
			bridge->mStat.mTotalBytes < 0 ||
			bridge->mStat.mErrorPkts < 0 ||
			bridge->mStat.mErrorBytes < 0)
		{
			sprintf(local, "Bridge Stat incorrect: %d. Stat = %d, %d, %d, %d\n", 
				bridge->mBridgeId, 
				(int)bridge->mStat.mTotalPkts, 
				(int)bridge->mStat.mTotalBytes, 
				(int)bridge->mStat.mErrorPkts, 
				(int)bridge->mStat.mErrorBytes);
			aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
			ret = eAosRc_BridgeHcErr;
		}

		if (bridge->mCrtDev)
		{
			// 
			// Make sure mCrtDev is one of the devices in the list
			//
			struct aosFwdDev *fdev = bridge->mDevInfo;
			int ss = 0;
			while (fdev && ss++ < eAosMaxDev+5 && 
				fdev != bridge->mCrtDev) fdev = fdev->mNext;
			if (!fdev)
			{
				sprintf(local, "CrtDev invalid: %d. Reset\n", 
					bridge->mBridgeId);
				aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
				bridge->mCrtDev = 0;
				bridge->mCrtHit = 0;
				bridge->mCrtWeight = 0;
			}
			ret = eAosRc_BridgeHcErr;
		}

		numDevs++;
		prev = fwddev;
		fwddev = fwddev->mNext;
	}	

	if (step >= eAosMaxDev + 5)
	{
		sprintf(local, "A loop is found: %d. Step = %d\n", 
			bridge->mBridgeId, step);
		aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
		ret = eAosRc_BridgeHcErr;
	}
		
	if (numDevs != bridge->mNumDevs)
	{
		sprintf(local, "NumDevs incorrect. Expected: %d. Actual: %d. "
			"Corrected\n", bridge->mBridgeId, numDevs);
		aosCheckAndCopy(buffer, index, buflen, local, strlen(local));
		bridge->mNumDevs = numDevs;
		ret = eAosRc_BridgeHcErr;
	}

	return ret;
}


int aosBridge_addDevCli(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms, 
						char *errmsg, 
						const int errlen)
{
	int ret;
	int bridgeId = parms->mIntegers[0];
	int weight = parms->mIntegers[1];
	char *side = parms->mStrings[0];
	char *devname = parms->mStrings[1];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIdx = 0;
	unsigned int optlen = *length;

	ret = aosBridge_addDev(bridgeId, devname, weight, side);
	ret |= aosBridge_healthcheck(rsltBuff, &rsltIdx, optlen);
	*length = rsltIdx;
	return ret;
}


int aosBridge_delDevCli(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms, 
						char *errmsg, 
						const int errlen)
{
	int bridgeId = parms->mIntegers[0];
	char *side = parms->mStrings[0];
	char *dev = parms->mStrings[1];
	unsigned int optlen = *length;
	int ret;
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIdx = 0;

	ret = aosBridge_delDev(bridgeId, side, dev);
	ret |= aosBridge_healthcheck(rsltBuff, &rsltIdx, optlen);
	*length = rsltIdx;
	return ret;
}


// ========================= Packet Handling Functions ==================
// 
// If not bridged, return eAosRc_Continue;
//
int aosBridge_proc(struct sk_buff *skb)
{
    int ret;
    struct aosBridgeDef *bridge;

    aosKernelLock(aosBridgeLock);
    bridge = sgAosBridgeData[skb->dev->ifindex];
    if (!bridge)
	{
    	aosKernelUnlock(aosBridgeLock);
		return eAosRc_Continue;
	}

    ret = bridge->mForward(bridge, skb);
    aosKernelUnlock(aosBridgeLock);
	return ret;
}


// 
// This is the function called when a packet is received. The function 
// will determine whether the packet should be bridged. If yes, it will 
// forward the packet based on the definition of the bridge, returns 0 
// and 'bridged' is set to 1. If the packet is not bridged, it returns 
// 0 and 'bridged' is set to 0. Under error conditions, it 
// returns non-zero value. 
//
// This function implements the round robin forwarding. The call flow
// is:
//	1. Check whether the packet is a local packet. If yes, do not 
//	   bridge it. Pass it back to the stack.
//  2. Check whether it needs to move to the next device, if multiple
//     devices are used. The decision is made based on the weighted
//	   round robin algorithm.
//  3. Bridge the packet.
//  4. If multicaster is not null, clone a packet and passes it to 
//	   the multicaster.
//
// NOTE: Layer 2 header has been stripped off. skb->data points to
//  	Layer 3 header.
//
// NOTE: If this function returns error, the packet is dropped. Do not 
// use it anymore.
//
// NOTE: It only handles:
//		MAC
//		IP
int aosBridge_procWrr(struct aosBridgeDef *self, struct sk_buff *skb)
{
	// 
	// The bridge is valid. The bridge contains a number of outbound
	// devices. 
	// 
	int ret = 0;
	struct sk_buff *cskb = 0;
	struct aosBridgeDef *bridged = self->mPeer;

	// 
	// Need to check whether the packet is for this machine. If yes, 
	// do not bridge it. A packet is for this machine if its destination 
	// MAC is on this machine or its destination IP address is a local
	// IP address.
	//
	// skb->data should point to Layer 3 header. All we care is to check
	// whether we should proceed based on the destination address, which
	// is available in Layer 3 header.
	//
	// if (skb->protocol == __constant_htons(ETH_P_IP))
	// {
		// 
		// It is an IP packet. 
		// 
		// char *daddr = (char *)skb->nh.iph;
		// struct aosRule *rule = 
		// 	aosDestCtlData[daddr[0]][daddr[1]][daddr[2]][daddr[3]];
		// if (rule && (ret = rule->mProc(rule, skb)) != eAosRc_ContAosPath)
		// {
          //   return ret;
        // }
	// }

	// 
	// Since this is a WRR, it determines whether to move the device.
	//
	bridged->mCrtHit++;
	if (bridged->mNumDevs > 1 && bridged->mCrtHit >= bridged->mCrtWeight)
	{
		// 
		// Go to the next one.
		//
		bridged->mCrtDev = (bridged->mCrtDev->mNext)?
			bridged->mCrtDev->mNext:bridged->mDevInfo;
		bridged->mCrtHit = 1;
	}

	// 
	// Note that we do not modify the source and destination MAC address. 
	// All it does is to forward the packet through the specified 
	// outbound device.
	// 

	// Do we need to release the dev before reassigning the new device?

	skb->dev = bridged->mCrtDev->mDev;

	// 
	// Note that all bridged packets should be receiving skbs. When a 
	// driver receives an skb, it strips off the Ether header. In other
	// word, skb->data - skb->head >= ETH_HLEN now. If we want to put
	// the packet back to the wire, it is important to move the pointers
	// back. This is done by the following statement.
	//
	if (skb->mac.raw != skb->data)
	{
		if (skb->data - skb->head < ETH_HLEN)
		{
			// 
			// This is not a correct skb. Drop it
			// 
			kfree_skb(skb);
			aosAlarmRated(eAosAlarmR_BridgedSkbNotCorrect, 1, 
				(unsigned int)skb->head, (unsigned int)skb->data, 0, 0);
			bridged->mStat.mErrorPkts++;
			bridged->mStat.mErrorBytes += skb->len;
			bridged->mCrtDev->mStat.mErrorPkts++;
			bridged->mCrtDev->mStat.mErrorBytes += skb->len;
			return eAosRc_PktConsumed;
		}
    
		skb_push(skb, ETH_HLEN);
		skb->mac.raw = skb->data;
	}

	if (bridged->mCaster)
	{
		cskb = skb_clone(skb, GFP_KERNEL);
	}

	// 
	// Send the packet. No matter what happened, the packet will be
	// gone after return.
	//
	ret = dev_queue_xmit(skb);

	if (bridged->mCaster)
	{
		if (cskb)
		{
			bridged->mCaster->mForward(bridged->mCaster, cskb);
		}
		else
		{
			// 
			// Failed to clone the packet. This is a memory error.
			//
			aosAlarmRated(eAosAlarmR_MemErr, 3, 0, 0, 0, 0);
		}
	}

	// 
	// The statistics. Note that 'ret == 0' does not guarantee that the
	// packet is sent successfully, but that is the best we can guess. 
	// If more accurate statistics is needed, we will have to go to the
	// driver to determine whether a packet was actually sent. For now, 
	// we just assume 'ret == 0' means the packet was sent.
	//
	if (ret == 0)
	{
		bridged->mStat.mTotalPkts++;
		bridged->mStat.mTotalBytes += skb->len;
		bridged->mCrtDev->mStat.mTotalPkts++;
		bridged->mCrtDev->mStat.mTotalBytes += skb->len;
	}
	else
	{
		bridged->mStat.mErrorPkts++;
		bridged->mStat.mErrorBytes += skb->len;
		bridged->mCrtDev->mStat.mErrorPkts++;
		bridged->mCrtDev->mStat.mErrorBytes += skb->len;
	}

	return ret;
}


// 
// This function is the same as 'procWrr' except that it supports
// IP flows. 
//
int aosBridge_procWrrIpf(struct aosBridgeDef *self, struct sk_buff *skb)
{
	// 
	// The bridge is valid. The bridge contains a number of outbound
	// devices. 
	// 
	int ret = 0;
	struct sk_buff *cskb = 0;
	struct aosBridgeDef *bridged = self->mPeer;

	// 
	// Need to check whether the packet is for this machine. If yes, 
	// do not bridge it. A packet is for this machine if its destination 
	// MAC is on this machine or its destination IP address is a local
	// IP address.
	//
	// This is implemented through a 6-dimensional array. 
	//
	// Not implemented yet!!!
	//
	// if (local packet)
	// 

	bridged->mCrtHit++;
	if (bridged->mNumDevs > 1 && bridged->mCrtHit >= bridged->mCrtWeight)
	{
		// 
		// Go to the next one.
		//
		bridged->mCrtDev = (bridged->mCrtDev->mNext)?
			bridged->mCrtDev->mNext:bridged->mDevInfo;
		bridged->mCrtHit = 1;
	}

	// 
	// Note that we do not modify the source and destination MAC address. 
	// All it does is to forward the packet through the specified 
	// outbound device.
	// 

	// Do we need to release the dev before reassigning the new device?

	skb->dev = bridged->mCrtDev->mDev;

	// 
	// Note that all bridged packets should be receiving skbs. When a 
	// driver receives an skb, it strips off the Ether header. In other
	// word, skb->data - skb->head >= ETH_HLEN now. If we want to put
	// the packet back to the wire, it is important to move the pointers
	// back. This is done by the following statement.
	//
	if (skb->mac.raw != skb->data)
	{
		if (skb->data - skb->head < ETH_HLEN)
		{
			// 
			// This is not a correct skb. Drop it
			// 
			kfree_skb(skb);
			return aosAlarmRated(eAosAlarmR_BridgedSkbNotCorrect, 1, 
				(unsigned int)skb->head, (unsigned int)skb->data, 0, 0);
		}
    
		skb_push(skb, ETH_HLEN);
		skb->mac.raw = skb->data;
	}

	if (bridged->mCaster)
	{
		cskb = skb_clone(skb, GFP_KERNEL);
	}

	// 
	// Send the packet
	//
	ret = dev_queue_xmit(skb);

	if (bridged->mCaster)
	{
		if (cskb)
		{
			bridged->mCaster->mForward(bridged->mCaster, cskb);
		}
		else
		{
			// 
			// Failed to clone the packet. This is a memory error.
			//
			aosAlarmRated(eAosAlarmR_MemErr, 3, 0, 0, 0, 0);
		}
	}

	// 
	// The statistics. Note that 'ret == 0' does not guarantee that the
	// packet is sent successfully, but that is the best we can guess. 
	// If more accurate statistics is needed, we will have to go to the
	// driver to determine whether a packet was actually sent. For now, 
	// we just assume 'ret == 0' means the packet was sent.
	//
	if (ret == 0)
	{
		bridged->mStat.mTotalPkts++;
		bridged->mStat.mTotalBytes += skb->len;
		bridged->mCrtDev->mStat.mTotalPkts++;
		bridged->mCrtDev->mStat.mTotalBytes += skb->len;
	}
	else
	{
		bridged->mStat.mErrorPkts++;
		bridged->mStat.mErrorBytes += skb->len;
		bridged->mCrtDev->mStat.mErrorPkts++;
		bridged->mCrtDev->mStat.mErrorBytes += skb->len;
	}

	return ret;
}

#endif

