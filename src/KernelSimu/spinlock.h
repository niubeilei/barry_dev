////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: spinlock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_spinlock_h
#define Omn_aos_KernelSimu_spinlock_h

#ifdef AOS_KERNEL_SIMULATE

#include <KernelSimu/aosKernelMutex.h>

#define read_lock_bh(lock)
#define read_unlock_bh(lock)
#define spin_lock_irqsave(x, y) y = 0
#define spin_unlock_irqrestore(x, y) y = 1


#define SPIN_LOCK_UNLOCKED (spinlock_t)  {} 

#else
#include <linux/spinlock.h>
#endif

#endif

