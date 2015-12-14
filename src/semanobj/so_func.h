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
// 12/17/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semanobj_funcso_h
#define aos_semanobj_funcso_h

#include "semanobj/so_type.h"
#include "semanobj/so.h"
#include "util_c/types.h"
#include "util_c/loc.h"

#define AOS_FUNCSO_MEMBER_FUNC_INIT						\
	aos_funcso_func_to_call, 							\
	aos_funcso_func_called,								\
	aos_funcso_func_entered,							\
	aos_funcso_func_exited

struct aos_funcso;

#define AOS_FUNCSO_MEMBER_FUNC_DECL							\
	int (*func_to_call)(struct aos_funcso *obj, 			\
						const char * const filename, 		\
						const int lineno, 					\
						aos_time_t *start_time);		\
	int (*func_called)(struct aos_funcso *obj, 				\
						const char * const filename, 		\
						const int lineno, 					\
						const aos_time_t start_time);		\
	int (*func_entered)(struct aos_funcso *obj, 			\
						const char * const filename, 		\
						const int lineno, 					\
						aos_time_t *start_time);		\
	int (*func_exited)(struct aos_funcso *obj, 				\
						const char * const filename, 		\
						const int lineno, 					\
						const aos_time_t start_time, 		\
						char * const exit_point)



typedef struct aos_funcso_class
{
	AOS_SO_MEMBER_FUNC_DECL;
	AOS_FUNCSO_MEMBER_FUNC_DECL;
} aos_funcso_class_t;

typedef struct aos_funcso_stat
{
	u32				num_calls;
	u32				dur1_max;
	aos_location_t	dur1_max_loc;
	u64				dur1_total;

	u32				dur2_max;
	aos_location_t	dur2_max_loc;
	u64				dur2_total;

} aos_funcso_stat_t;

typedef struct aos_funcso
{
	aos_funcso_class_t *member_funcs;

	char				log_flag;
	char				stat_flag;
	aos_funcso_stat_t	stat;
} aos_funcso_t;


extern aos_funcso_t * aos_create_funcso(
  				char *filename, 
  				int lineno, 
  				void *inst, 
  				char *type, 
  				u32 size);

#endif // End of Include

