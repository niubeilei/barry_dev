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
//   
//
// Modification History:
// 01/21/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "thread_c/thread.h"

#include "alarm_c/alarm.h"
#include "porting_c/mutex.h"
#include "porting_c/port_thread.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/rc_obj.h"
#include <pthread.h>

static aos_lock_t sg_lock;
static int sg_thread_lid[AOS_MAX_THREADS] = {0};
AOS_DECLARE_RCOBJ_LOCK;

u16 aos_get_current_tlid()
{
	int tid = pthread_self();
	u16 idx = (u16)(tid & (AOS_MAX_THREADS-1));
	int tries = 0;
	aos_lock(&sg_lock);
	while (tries++ < AOS_MAX_THREADS && sg_thread_lid[idx])
	{
		if (sg_thread_lid[idx] == tid) 
		{
			aos_unlock(&sg_lock);
			return idx;
		}

		idx++;
		if (idx >= AOS_MAX_THREADS) idx = 0;
	}

	aos_assert_r(tries < AOS_MAX_THREADS, 0);

	// 
	// No logic ID has been assigned to the thread. Assign it.
	//
	sg_thread_lid[idx] = tid;
	aos_unlock(&sg_lock);
	return idx;
}


int aos_thread_hold(aos_thread_t *thread)
{
	AOS_RC_OBJ_HOLD(thread);
}

 
int aos_thread_put(aos_thread_t *thread)
{
	AOS_RC_OBJ_PUT(thread);
}

 
int aos_thread_release_memory(aos_thread_t *thread)
{
	aos_assert_r(thread, -1);
	if (thread->name) aos_free(thread->name);
	thread->name = 0;
	return 0;
}

 
int aos_thread_destroy(aos_thread_t *thread)
{
	aos_assert_r(thread, -1);
	aos_assert_r(!thread->mf->release_memory(thread), -1);
	aos_free(thread);
	return 0;
}


int aos_thread_identity_check(aos_thread_t *thread)
{
	aos_assert_r(thread, -1);
	return thread->magic == AOS_THREAD_MAGIC;
}


static aos_thread_return_type_t 
aos_thread_generic_thread_func(aos_thread_parm_type_t obj)
{
	// ((aos_thread_t *)obj)->thread_func((aos_thread_t *)obj);
	aos_thread_t *thread = (aos_thread_t *)obj;
	aos_assert_r(thread, 0);
	aos_assert_r(thread->thread_func, 0);
	thread->thread_func(thread);
	return 0;
}

static aos_thread_mf_t sg_mf = 
{
	aos_thread_hold,
	aos_thread_put,
	aos_thread_release_memory,
	aos_thread_destroy
};


int aos_thread_init(aos_thread_t *thread)
{
	memset(thread, 0, sizeof(aos_thread_t));
	thread->mf = &sg_mf;
	thread->magic = AOS_THREAD_MAGIC;
	return 0;
}


aos_thread_t *aos_thread_create(
		const char * const name, 
		const int logic_id, 
		aos_thread_func_t thread_func,
		void *user_data,
		const int managed_flag,
		const int is_critical_thread, 
		const int is_high_priority)
{
	aos_thread_t *thread = aos_malloc(sizeof(aos_thread_t));
	aos_assert_r(thread, 0);
	aos_assert_r(!aos_thread_init(thread), 0);

	thread->logic_id = logic_id;
	thread->user_data = user_data;
	thread->is_managed = managed_flag;
	thread->is_critical_thread = is_critical_thread;
	thread->is_high_priority = is_high_priority;
	thread->thread_func = thread_func;
	if (name) aos_assert_r(!aos_str_set(&thread->name, name, strlen(name)), 0);

	aos_assert_r(!aos_create_thread(aos_thread_generic_thread_func, 
			thread, &thread->thread_id, is_high_priority), 0);
	return thread;
}



