////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Mutex.h
// Description:
//	This header file defines the following macros:
//		AOS_DECLARE_MUTEX(x)
//		AOS_INIT_MUTEX(x)
//		aos_lock(x)
//		aos_unlock(x)
//
//		AOS_DECLARE_SEMAPHORE(x)
//		AOS_INIT_SEMAPHORE(name, num)
//
//	To create and use semaphore:
//	int example()
//	{
//		aos_semaphore_t sema;
//		AOS_INIT_SEMAPHORE(sema, 2);
//		
//		aos_lock(sema);
//		// This is the critical section	
//		aos_unlock(sema);
//		...
//		return 0;
//	}
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelUtil_Mutex_h
#define aos_KernelUtil_Mutex_h

#ifdef __KERNEL__
//
// The kernel portion 
//
#include <asm/semaphore.h>

typedef struct semaphore aos_mutex_t;
typedef struct semaphore aos_semaphore_t;

#ifndef AOS_DECLARE_MUTEX
#define AOS_DECLARE_MUTEX(name) struct semaphore name
#endif

#ifndef AOS_DECLARE_SEMAPHORE
#define AOS_DECLARE_SEMAPHORE(name) struct semaphore name
#endif

#ifndef AOS_INIT_MUTEX
#define AOS_INIT_MUTEX(name)							\
do 														\
{														\
    atomic_set(&name.count, 1);							\
    name.sleepers   = 0;								\
    name.wait.lock = SPIN_LOCK_UNLOCKED;				\
    name.wait.task_list.next = &name.wait.task_list;	\
    name.wait.task_list.prev = &name.wait.task_list;	\
} while (0)
#endif

#ifndef AOS_INIT_SEMAPHORE
#define AOS_INIT_SEMAPHORE(name, num)					\
do 														\
{														\
    atomic_set(&name.count, num);						\
    name.sleepers   = 0;								\
    name.wait.lock = SPIN_LOCK_UNLOCKED;				\
    name.wait.task_list.next = &name.wait.task_list;	\
    name.wait.task_list.prev = &name.wait.task_list;	\
} while (0)
#endif

#ifndef aos_lock
#define aos_lock(x) down(&(x))
#endif

#ifndef aos_unlock
#define aos_unlock(x) up(&(x))
#endif 


#else
// Userland 
#include "Porting/Mutex.h"

// typedef OmnMutexType aos_mutex_t;
typedef pthread_mutex_t aos_mutex_t;

#define AOS_DECLARE_MUTEX(name) OmnMutexType name

#ifndef AOS_INIT_MUTEX
#define AOS_INIT_MUTEX(x) OmnInitMutex(x)
#endif

#ifndef aos_lock
#define aos_lock(x) OmnMutexLock(x)
#endif

#ifndef aos_unlock
#define aos_unlock(x) OmnMutexUnlock(x)
#endif 


#endif

#endif

