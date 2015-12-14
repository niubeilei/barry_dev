////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file defines the mutex related functions for C.
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "porting_c/mutex.h"




#ifdef OMN_PLATFORM_UNIX
#include <pthread.h>

#include "alarm_c/alarm.h"
#include "porting_c/sleep.h"
#include "util2/global_data.h"
#include "util_c/memory.h"
#include <stdio.h>

// inline int aos_init_lock(aos_lock_t *lock) {return pthread_mutex_init(lock, 0);}
// inline int aos_destroy_lock(aos_lock_t *lock) {return pthread_mutex_destroy(lock);}
// inline int aos_lock(aos_lock_t *lock) {return pthread_mutex_lock(lock);}
// inline int aos_unlock(aos_lock_t *lock) {return pthread_mutex_unlock(lock);}


// 
// Description
// This function creates a lock in an exclusive way, which 
// means that the same lock will not be created by multiple
// threads at the same time. This is ensured by the use of
// a global lock. If the global lock
// is not initialized at the time this function is called, 
// it will wait until the lock is initialized. Note that
// the global lock should be initialized the first 
// thing an application starts. 
//
int aos_create_lock_exclusively(aos_lock_t **lock)
{
	aos_assert_raw(aos_global_lock_is_initialized() == 1);

	while (aos_global_lock_is_initialized() != 1)
	{
		aos_sleep(1);
		aos_assert_raw(aos_global_lock_is_initialized() == 1);
	}

	aos_global_lock();
	if (*lock)
	{
		// This means the lock might have been created by
		// another thread. Do nothing
		aos_global_unlock();
		return 0;
	}

	aos_lock_t *the_lock = (aos_lock_t*)aos_malloc(sizeof(aos_lock_t));
	aos_assert_g(the_lock, cleanup);
	memset(the_lock, 0, sizeof(aos_lock_t));
	aos_assert_g(!aos_init_lock(the_lock), cleanup);
	*lock = the_lock;
	aos_global_unlock();
	return 0;

cleanup:
	if (the_lock) aos_free(lock);
	aos_global_unlock();
	return -1;
}
#elif defined(OMN_PLATFORM_MICROSOFT)
#include "Porting/GetErrno.h"
#include <winsock2.h>
#endif


