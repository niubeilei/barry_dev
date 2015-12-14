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
// 02/21/2008: Copied from aosUtil by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_timer_proc_h
#define aos_util_timer_proc_h

#include "util_c/dyn_array.h"


typedef int (*aos_timer_proc_serialize_t)(
		struct aos_timer_proc *tp, 
		struct aos_xml_node *parent);

typedef int (*aos_timer_proc_deserialize_t)(
		struct aos_timer_proc *tp, 
		struct aos_xml_node *node);

typedef int (*aos_timer_proc_destroy_t)(
		struct aos_timer_proc *tp);

typedef int (*aos_timer_proc_release_mem_t)(
		struct aos_timer_proc *tp);

typedef const char * (*aos_timer_proc_dump_t)(
		struct aos_timer_proc *tp);

#define AOS_TIMER_PROC_MEMFUNC_DECL						\
	aos_timer_proc_serialize_t 			serialize;		\
	aos_timer_proc_deserialize_t 		deserialize;	\
	aos_timer_proc_destroy_t 			destroy;		\
	aos_timer_proc_release_mem_t 		release_mem;	\
	aos_timer_proc_dump_t 				dump

#define AOS_TIMER_PROC_MEMDATA_DECL						\
	int						msec;						\
	char *					name;						\
	int						timer_id;					\
	aos_timer_func_t		callback;					\
	struct aos_action **	actions;					\
	aos_dyn_array_t			actions_array


typedef struct aos_timer_proc_mf
{
	AOS_TIMER_PROC_MEMFUNC_DECL;
} aos_timer_proc_mf_t;

typedef struct aos_timer_proc
{
	aos_timer_proc_mf_t *mf;

	AOS_TIMER_PROC_MEMDATA_DECL;
} aos_timer_proc_t;

extern int aos_timer_proc_init(aos_timer_proc_t *tp);
extern aos_timer_proc_t *aos_timer_proc_create_xml(struct aos_xml_node *node);

#endif

