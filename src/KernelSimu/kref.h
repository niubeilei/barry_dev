////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: kref.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_kref_h
#define Omn_aos_KernelSimu_kref_h

#ifdef AOS_KERNEL_SIMULATE

#include "KernelSimu/aosKernelMutex.h"


struct kref {
	OmnMutexType	lock;
	int				refcount;
};

void kref_init(struct kref *kref);
void kref_get(struct kref *kref);
void kref_put(struct kref *kref, void (*release) (struct kref *kref));


#else
#include <linux/kref.h>
#endif

#endif

