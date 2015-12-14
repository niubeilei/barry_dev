////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSkbRecv.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosSkbRecv_h
#define aos_core_aosSkbRecv_h

#include <KernelSimu/if_ether.h>
#include "aos/aosCoreComm.h"

struct net_device;

struct aosSkbRecv
{
	struct aosSkbRecv *		mPrev;
	struct aosSkbRecv *		mNext;
	struct net_device *		mDev;
	unsigned int			mAddr;
	int						mWeight;
	unsigned char			mMac[ETH_ALEN];
	unsigned char			mMacGood;
	struct aosPktStat		mStat;
};

extern struct aosSkbRecv *
aosSkbRecv_constructor(struct net_device *dev,
                       unsigned int addr,
                       int weight,
                       int *ret);
extern int aosSkbRecv_destructor(struct aosSkbRecv *self);
#endif

