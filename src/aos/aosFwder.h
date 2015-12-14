////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosFwder.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosFwder_h
#define aos_core_aosFwder_h

#include <KernelSimu/if_ether.h>

#include "aos/KernelEnum.h"
#include "aos/aosCoreComm.h"
#include "aos/aosSkbRecv.h"

struct sk_buff;

enum
{
	eAosMaxFwderRecv = 101
};

#define eAosDefaultDevMethod eAosMethod_RR
#define eAosDefaultRecvMethod eAosMethod_RR

struct aosFwder
{
	int					mFwdId;
	unsigned short		mFlags;
//	struct aosFwdList * mPrev;
//	struct aosFwdList * mNext;
//	struct aosFwdList *	mList;
	struct aosFwder *	mPrev;
	struct aosFwder *	mNext;
	struct aosPktStat	mStat;

	struct aosFwdDev*	mDevHead;
	struct aosFwdDev *	mCrtDev;
	int					mCrtDevWeight;
	int					mDevMethod;
	int					mNumDevs;
	int					mDevHit;

	struct aosSkbRecv *	mRecvHead;
	struct aosSkbRecv *	mCrtRecv;
	int					mCrtRecvWeight;
	int					mRecvMethod;
	int					mNumRecvs;
	int					mRecvHit;
	int					mUseOwnMac;		// If 1, will use outbound device mac

	int (*mForward)(struct aosFwder *self, struct sk_buff *skb);
};

extern int aosFwder_clear(struct aosFwder *self);
extern struct aosFwder * 
aosFwder_constructor(unsigned short flags,
                     int devMethod,
                     int recvMethod,
                     int useOwnMac,
                     int numDev,
                     char names[eAosMaxDev][eAosMaxDevNameLen],
                     int devWeight[],
                     int numRecv, unsigned int addr[], int recvWeight[],
                     int *ret);
extern int aosFwder_destructor(struct aosFwder *fwder);

#endif

