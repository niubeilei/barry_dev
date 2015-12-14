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
// 01/21/2008: Moved some functions from Random/RandomUtil.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_random_random_util_h
#define aos_random_random_util_h

#include "util_c/types.h"

#ifdef __cplusplus
extern "C" {
#endif 

extern u32 aos_next_u32(const u32 min, const u32 max);
extern int aos_next_int(const int min, const int max);
extern int64_t aos_next_int64(const int64_t min, const int64_t max);
extern int aos_next_pct(const u32 pct);
extern char aos_next_hex_char();

#ifdef __cplusplus
}
#endif

#endif

