////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosFwdDev.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_core_aosFwdDev_h
#define aos_aos_core_aosFwdDev_h

#include "aos/aosCoreComm.h"

struct net_device;


struct aosFwdDev
{
	struct aosFwdDev *	mPrev;
	struct aosFwdDev *	mNext;
	struct net_device *	mDev;
	int					mWeight;
	struct aosPktStat	mStat;
};


extern int aosFwdDev_destructor(struct aosFwdDev *self);
extern struct aosFwdDev * aosFwdDev_constructor(struct net_device *dev,
                      int weight,
                      int *ret);
extern int aosFwdDev_reset(struct aosFwdDev *self);


#endif
