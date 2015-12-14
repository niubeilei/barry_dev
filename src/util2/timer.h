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
// 03/14/2008 Copied from Util1 CPP version by Chen Ding
////////////////////////////////////////////////////////////////////////////

#ifndef aos_util2_timer_h
#define aos_util2_timer_h

#include "util_c/list.h"
#include "util_c/types.h"
#include <sys/time.h>
#include <time.h>

typedef int (*aos_timer_callback_t)(
		const u32 timer_id, 
		void *user_data);

extern int aos_timer_start_timer(
		const u32 msec, 
		u32 *timer_id,
		aos_timer_callback_t callback, 
		void *user_data);

extern int aos_timer_cancel_timer(const u32 timer_id);

typedef struct aos_timer
{
	aos_list_head_t 		link;
   	u32						timer_id;
	aos_timer_callback_t	callback;
   	struct timeval 			end_time;
	void *					user_data;
} aos_timer_t;

#endif


