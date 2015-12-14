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
#ifndef Omn_porting_c_mutex_h
#define Omn_porting_c_mutex_h





#ifdef OMN_PLATFORM_UNIX
#include "alarm_c/alarm.h"
#include "util_c/memory.h"
#include <pthread.h>

typedef pthread_mutex_t aos_lock_t;

static inline int aos_init_lock(aos_lock_t *lock) 
{
	return pthread_mutex_init(lock, 0);
}


static inline int aos_lock_create(aos_lock_t **lock)
{
	aos_assert_r(lock, -1);
	*lock = (aos_lock_t *)malloc(sizeof(aos_lock_t));
	aos_assert_r(*lock, -1);
	aos_init_lock(*lock);
	return 0;
}


static inline int aos_destroy_lock(aos_lock_t *lock) 
{
	return pthread_mutex_destroy(lock);
}

static inline int aos_lock(aos_lock_t *lock) 
{
	return pthread_mutex_lock(lock);
}

static inline int aos_unlock(aos_lock_t *lock) 
{
	return pthread_mutex_unlock(lock);
}

// extern inline int aos_init_lock(aos_lock_t *lock);
// extern inline int aos_destroy_lock(aos_lock_t *lock);
// extern inline int aos_lock(aos_lock_t *lock);
// extern inline int aos_unlock(aos_lock_t *lock);

#ifdef __cplusplus
extern "C" {
#endif

extern int aos_create_lock_exclusively(aos_lock_t **lock);

#ifdef __cplusplus
}
#endif

#elif defined(OMN_PLATFORM_MICROSOFT)
#include "Porting/GetErrno.h"
#include <winsock2.h>
#endif

#endif

