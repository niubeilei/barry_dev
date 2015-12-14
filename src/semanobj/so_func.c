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
// Modification History:
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanobj/so_func.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Memory.h"
#include "porting_c/mutex.h"
#include "semanobj/so_func_priv.h"
#include "util_c/time.h"




static aos_funcso_class_t sg_member_funcs = 
{
	AOS_SO_MEMBER_FUNC_INIT,
	AOS_FUNCSO_MEMBER_FUNC_INIT
};

aos_funcso_t * aos_create_funcso(
  				char *filename, 
  				int lineno, 
  				void *inst, 
  				char *type, 
  				u32 size)
{
	aos_funcso_t *obj = (aos_funcso_t *)aos_malloc(sizeof(aos_funcso_t));
	aos_assert_r(obj, NULL);

	obj->member_funcs = &sg_member_funcs;

	return obj;
}


// 
// A function is to be called. 
//
int aos_funcso_func_to_call(struct aos_funcso *obj, 
						    const char * const filename, 
						    const int lineno, 
							aos_time_t *start_time)
{
	*start_time = aos_get_crt_time();
	return 0;
}

// 
// A function is entered. 
//
int aos_funcso_func_entered(struct aos_funcso *obj, 
						    const char * const filename, 
						    const int lineno, 
							aos_time_t *start_time)
{
	*start_time = aos_get_crt_time();
	return 0;
}


// 
// A function has been called and returned from the function
//
int aos_funcso_func_called(struct aos_funcso *obj, 
						   const char * const filename, 
						   const int lineno, 
						   const aos_time_t start_time)
{
	if (obj->stat_flag) aos_funcso_stat(obj, filename, lineno, start_time);
	if (obj->log_flag)  aos_funcso_log(obj, filename, lineno, start_time);

	return 0;
}


// 
// A function has exited from a specific exit point
//
int aos_funcso_func_exited(struct aos_funcso *obj, 
						   const char * const filename, 
						   const int lineno, 
						   const aos_time_t start_time, 
						   char * const exit_point)
{
	if (obj->stat_flag) aos_funcso_stat(obj, filename, lineno, start_time);
	if (obj->log_flag)  aos_funcso_log(obj, filename, lineno, start_time);

	return 0;
}


int aos_funcso_stat(aos_funcso_t *obj, 
					const char * const filename, 
					const int lineno, 
					const aos_time_t start_time)
{
	u32 dur;
	aos_time_t end_time = aos_get_crt_time();
	obj->stat.num_calls++;

	dur = end_time - start_time;
	obj->stat.dur1_total += dur;
	if (dur > obj->stat.dur1_max) 
	{
		obj->stat.dur1_max = dur;
		aos_set_location(&(obj->stat.dur1_max_loc), filename, lineno);
	}

	return 0;
}


int aos_funcso_log(aos_funcso_t *obj, 
				   const char * const filename, 
				   const int lineno, 
				   const aos_time_t start_time)
{
	return 0;
}



