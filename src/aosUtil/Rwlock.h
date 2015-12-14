////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Rwlock.h
// Description:
//	This header file defines the following macros:
//		AOS_RWLOCK_TYPE
//		AOS_DECLARE_RWLOCK(x)
//		AOS_INIT_RWLOCK(x)
//		aos_write_lock_bh(x)
//		aos_write_unlock_bh(x)
//		aos_read_lock(x)
//		aos_read_unlock(x)   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelUtil_Rwlock_h
#define aos_KernelUtil_Rwlock_h

#ifdef __KERNEL__
//
// The kernel portion 
//
#include <linux/spinlock.h>
#ifndef aos_rwlock_t
#define aos_rwlock_t rwlock_t
#endif

#ifndef AOS_DECLARE_RWLOCK
#define AOS_DECLARE_RWLOCK(name) rwlock_t name
#endif

#ifndef AOS_INIT_RWLOCK
#define AOS_INIT_RWLOCK(name)			\
do 										\
{										\
    name = RW_LOCK_UNLOCKED;			\
} while (0)
#endif


#ifndef aos_write_lock_bh
#define aos_write_lock_bh(x) write_lock_bh(&(x))
#endif

#ifndef aos_write_unlock_bh
#define aos_write_unlock_bh(x) write_unlock_bh(&(x))
#endif 

#ifndef aos_read_lock
#define aos_read_lock(x) read_lock(&(x))
#endif

#ifndef aos_read_unlock
#define aos_read_unlock(x) read_unlock(&(x))
#endif 

#else

// Userland 
#include "Porting/Mutex.h"
#ifndef aos_rwlock_t
#define aos_rwlock_t pthread_mutex_t
#endif

#ifndef AOS_DECLARE_RWLOCK
#define AOS_DECLARE_RWLOCK(name) pthread_mutex_t name
#endif

#ifndef AOS_INIT_RWLOCK
#define AOS_INIT_RWLOCK(x) OmnInitMutex(x)
#endif

#ifndef aos_write_lock_bh
#define aos_write_lock_bh(x) OmnMutexLock(x)
#endif

#ifndef aos_write_unlock_bh
#define aos_write_unlock_bh(x) OmnMutexUnlock(x)
#endif 

#ifndef aos_read_lock
#define aos_read_lock(x) OmnMutexLock(x)
#endif

#ifndef aos_read_unlock
#define aos_read_unlock(x) OmnMutexUnlock(x)
#endif 

#endif

#endif



