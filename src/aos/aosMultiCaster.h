////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMultiCaster.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_coreMultiCaster_h
#define aos_aos_coreMultiCaster_h

#include "aos/aosCoreComm.h"
#include "aos/KernelEnum.h"

struct aosFwder;
struct sk_buff;

enum 
{
	eAosMaxCaster = 256
};


struct aosMultiCaster
{
	unsigned short			mMcId;
	unsigned short			mFlags;
	int						mNumFwders;
	struct aosFwder *		mFwders;
	struct aosPktStat		mStat;
	int (*mForward)(struct aosMultiCaster *self, struct sk_buff *skb);
};

extern int aosMultiCaster_init(void);
extern int aosMultiCaster_clearAll(void);
extern int aosMultiCaster_isDefined(int mcId);
extern struct aosMultiCaster * aosMultiCast_constructor(unsigned char mcId,
                         unsigned short flags,
                         int *ret);
extern int
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
                        int recvWeight[]);
extern int aosMultiCaster_removeFwder(struct aosMultiCaster *self, int fwderId);
extern int aosMultiCaster_destructor(struct aosMultiCaster *self);

#endif

