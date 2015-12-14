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
// 12/15/2007: Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "util_c/time.h"

#include "alarm_c/alarm.h"
#include <sys/time.h>
#include <time.h>
#include "util_c/types.h"
#include "util_c/memory.h"
#include "util_c/rc.h"

aos_time_t aos_get_crt_time()
{
	struct timeval tp;
	aos_time_t tt;
	gettimeofday(&tp, 0);
	tt = tp.tv_sec;
	tt = (tt << 32) + tp.tv_usec;
	return tt;
}


int aos_get_crt_sec()
{
	struct timeval tp;
	gettimeofday(&tp, 0);
	return tp.tv_sec;
}


int aos_get_crt_usec()
{
	struct timeval tp;
	gettimeofday(&tp, 0);
	return tp.tv_usec;
}

int aos_measuretime_init(aos_time_measure_t *tm)
{
	aos_assert_r(tm, eAosRc_NullPtr);
	memset(tm, 0, sizeof(aos_time_measure_t));
	return 0;
}


int aos_measuretime_start(aos_time_measure_t *tm)
{
	struct timeval tv;
	aos_assert_r(tm, -eAosRc_NullPtr);
	gettimeofday(&tv, 0);
	tm->slots[0] = tv.tv_sec;
	tm->slots[0] = tm->slots[0] * (u64)1000000000 + tv.tv_usec;
	return 0;
}


int aos_measuretime_snapshot(aos_time_measure_t *tm)
{
	struct timeval tv;
	int index = tm->num_slots;
	aos_assert_r(tm, -eAosRc_NullPtr);
	aos_assert_r(index >= 0 && 
			index < AOS_TIME_MEASURE_MAX_SLOTS, -eAosRc_ProgramError);

	gettimeofday(&tv, 0);
	tm->slots[index] = tv.tv_sec;
	tm->slots[index] = tm->slots[index] * (u64)1000000000 + tv.tv_usec;
	return 0;
}

