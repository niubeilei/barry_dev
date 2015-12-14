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
#ifndef aos_util2_global_data_h
#define aos_util2_global_data_h

#include "porting_c/mutex.h"
#include "thread_c/thread.h"

#define AOS_THREAD_GLOBAL_MAX_INT 20
#define AOS_THREAD_GLOBAL_STR1_LEN 1024

#ifdef __cplusplus
extern "C" {
#endif

extern int aos_thread_global_int[AOS_MAX_THREADS][AOS_THREAD_GLOBAL_MAX_INT];
extern char *aos_thread_global_str1[AOS_MAX_THREADS];
extern void * aos_thread_global_void_ptr[AOS_MAX_THREADS];

extern char *aos_get_global_str1(int *len);
extern int aos_global_data_init();
extern int aos_global_lock_is_initialized();
extern int aos_global_lock();
extern int aos_global_unlock();

#ifdef __cplusplus
}
#endif

#endif

