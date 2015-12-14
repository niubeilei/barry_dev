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
#ifndef aos_thread_thread_h
#define aos_thread_thread_h

#define AOS_MAX_THREADS 1024
#define AOS_THREAD_MAGIC 62345263

#include "porting_c/port_thread.h"
#include "util_c/types.h"

typedef int (*aos_thread_hold_t)(
		struct aos_thread *event);
 
typedef int (*aos_thread_put_t)(
		struct aos_thread *event);
 
typedef int (*aos_thread_release_memory_t)(
		struct aos_thread *event);
 
typedef int (*aos_thread_destroy_t)(
		struct aos_thread *event);

typedef int (*aos_thread_identity_check_t)(
		struct aos_thread *event);

#define AOS_THREAD_MEMFUNC_DECL						\
	aos_thread_hold_t				hold;			\
	aos_thread_put_t				put;			\
	aos_thread_release_memory_t		release_memory;	\
	aos_thread_destroy_t			destroy

#define AOS_THREAD_MEMDATA_DECL						\
	char *					name;					\
	aos_thread_func_t		thread_func;			\
	int						logic_id;				\
	void *					user_data;				\
	char					is_managed;				\
	int						magic;					\
	char					is_critical_thread;		\
	char					is_high_priority;		\
	aos_thread_id_t			thread_id;				\
	int						is_destroying;			\
	int						ref_count;				\
	int						finished

typedef struct aos_thread_mf
{
	AOS_THREAD_MEMFUNC_DECL;
} aos_thread_mf_t;

typedef struct aos_thread
{
	aos_thread_mf_t *mf;

	AOS_THREAD_MEMDATA_DECL;
} aos_thread_t;


#ifdef __cplusplus
extern "C" {
#endif

extern u16 aos_get_current_tlid();

extern int aos_thread_init(aos_thread_t *thread);

extern aos_thread_t *aos_thread_create(
		const char * const name, 
		const int logic_id, 
		aos_thread_func_t thread_func,
		void *user_data,
		const int managed_flag,
		const int is_critical_thread, 
		const int is_high_priority);

#ifdef __cplusplus
}
#endif

#endif
