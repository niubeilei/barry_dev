////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelMutex.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelSimu_KernelMutex_h
#define aos_KernelSimu_KernelMutex_h

#ifdef __KERNEL__
#include <asm/semaphore.h>

#ifndef aosKernelMutex
#define aosKernelMutex struct semaphore
#endif

#ifndef aosKernelLock
#define aosKernelLock(x) down(&x)
#endif

#ifndef aosKernelUnlock
#define aosKernelUnlock(x) up(&x)
#endif 

#else
// Userland Portion
#include "Porting/Mutex.h"

#ifndef aosKernelMutex
#define aosKernelMutex OmnMutexType
#endif 

#ifdef OMN_PLATFORM_MICROSOFT
#define __SEMAPHORE_INITIALIZER(x, y) {0}
#elif OMN_PLATFORM_UNIX
#define __SEMAPHORE_INITIALIZER(x, y) {}
#endif

#define DECLARE_MUTEX(name) aosKernelMutex name


#ifndef aosKernelLock
#define aosKernelLock(x) OmnMutexLock(x)
#endif

#ifndef aosKernelUnlock
#define aosKernelUnlock(x) OmnMutexUnlock(x)
#endif 

// John, 12/18/2006
#ifndef aosKernelLockInit
#define aosKernelLockInit(x) 
#endif

#define spinlock_t	OmnMutexType

#endif

#endif

