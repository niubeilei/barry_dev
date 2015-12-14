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
#ifndef aos_util_time_h
#define aos_util_time_h

#include "util_c/types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern aos_time_t aos_get_crt_time();
extern int aos_get_crt_sec();
extern int aos_get_crt_usec();

#ifdef __cplusplus
}
#endif


#define AOS_TIME_MEASURE_MAX_SLOTS 30

typedef struct
{
	u64		slots[AOS_TIME_MEASURE_MAX_SLOTS];
	int		num_slots;
} aos_time_measure_t;

#endif


