////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Semaphore.h
// Description:
//	This header file defines the following macros:
//		AOS_DECLARE_SEMAPHORE(x)
//		AOS_INIT_SEMAPHORE(x)
//		aos_lock(x)
//		aos_unlock(x)
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_AosUtil_Semaphore_h
#define aos_AosUtil_Semaphore_h

#ifdef __KERNEL__
//
// The kernel portion 
//
#include <asm/semaphore.h>

#ifndef AOS_DECLARE_SEMAPHORE
#define AOS_DECLARE_SEMAPHORE(name) struct semaphore name
#endif

#ifndef AOS_INIT_SEMAPHORE
#define AOS_INIT_SEMAPHORE(name, n)								\
{																\
	.count		= ATOMIC_INIT(n),								\
	.sleepers	= 0,											\
	.wait		= __WAIT_QUEUE_HEAD_INITIALIZER((name).wait)	\
}

#ifndef aos_lock
#define aos_lock(x) down(&(x))
#endif

#ifndef aos_unlock
#define aos_unlock(x) up(&(x))
#endif 


#else
// Userland 
#include "Porting/Mutex.h"

#define AOS_DECLARE_SEMAPHORE(name) OmnMutex name

#ifndef AOS_INIT_SEMAPHORE
#ifdef OMN_PLATFORM_MICROSOFT
#define AOS_INIT_SEMAPHORE(x, n) {0}
#elif OMN_PLATFORM_UNIX
#define AOS_INIT_SEMAPHORE(x, n) {}
#endif
#endif

#ifndef aos_lock
#define aos_lock(x) OmnMutexLock(x)
#endif

#ifndef aos_unlock
#define aos_unlock(x) OmnMutexUnlock(x)
#endif 


#endif

#endif

