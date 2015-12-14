////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 01/21/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/global_data.h"

#include "alarm_c/alarm.h"
#include "thread_c/thread.h"
#include "util_c/memory.h"
#include "util2/clipboard.h"
#include "util2/global_data.h"


int aos_global_data_init();
int aos_thread_global_int[AOS_MAX_THREADS][AOS_THREAD_GLOBAL_MAX_INT];
void * aos_thread_global_void_ptr[AOS_MAX_THREADS];

char *aos_thread_global_str1[AOS_MAX_THREADS] = {0};


aos_lock_t *sg_lock = 0;
int		   sg_initialized = 0;

int aos_global_data_init()
{
	if (sg_initialized) return 0;

	sg_initialized = 1;
	sg_lock = aos_malloc(sizeof(*sg_lock));
	aos_init_lock(sg_lock);
	aos_assert_r(!aos_clipboard_init(), -1);
	return 0;
}


int aos_global_lock_is_initialized()
{
	return sg_initialized;
}


int aos_global_lock()
{
	aos_assert_r(sg_lock, -1);
	aos_lock(sg_lock);
	return 0;
}


int aos_global_unlock()
{
	aos_assert_r(sg_lock, -1);
	aos_unlock(sg_lock);
	return 0;
}


char *aos_get_global_str1(int *len)
{
	aos_assert_r(len, 0);
	u16 tlid = aos_get_current_tlid();
	*len = 0;
	if (!aos_thread_global_str1[tlid])
	{
		aos_thread_global_str1[tlid] = aos_malloc(AOS_THREAD_GLOBAL_STR1_LEN+1);
		aos_assert_r(aos_thread_global_str1[tlid], 0);
	}
	*len = AOS_THREAD_GLOBAL_STR1_LEN;
	return aos_thread_global_str1[tlid];
}

