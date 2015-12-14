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
// 01/21/2008 Moved some functions from Random/RandomUtil.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "random_c/random_util.h"

#include "alarm_c/alarm.h"
#include "util_c/time.h"
#include <stdlib.h>


int aos_next_pct(const u32 pct)
{
	int value = aos_next_int(1, 100);
    return (value <= pct)?1:0;
}


char aos_next_hex_char()
{
	// 
	// A hex character is [0-9, a-f, A-F].
	//
	int pct = aos_next_int(1, 100);
	if (pct < 33) return '0' + aos_next_int(0, 9);
	if (pct < 66) return 'a' + aos_next_int(0, 5);
	return 'A' + aos_next_int(0, 5);
}


void aos_random_init()
{
	 srand(aos_get_crt_usec());
}


u32 aos_next_u32(const u32 min, const u32 max)
{
	aos_assert_r(min <= max, min);
	u32 space = max - min;
	if (space == 0) return min;
	return rand() % space + min;
}


int aos_next_int(const int min, const int max)
{
	if (min == max) return min;

	aos_assert_r(min <= max, min);
	int space = max - min;
	return rand()%space + min;
}


int64_t aos_next_int64(const int64_t min, const int64_t max)
{
	aos_assert_r(min <= max, min);
	int64_t space = max - min;
	if (space == 0) return min;
	return rand()%space + min;
}




