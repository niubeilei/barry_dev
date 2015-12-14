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
#ifndef aos_util_util_h
#define aos_util_util_h

#include <limits.h>
#include "aosUtil/Types.h"


static inline int aos_is_binary(const int v)
{
	return (v == 0 || v == 1);
}

static inline int aos_check_int16(const int64_t v)
{
	return (v >= SHRT_MIN && v <= SHRT_MAX);
}

static inline int aos_check_int32(const int64_t v)
{
	return (v >= INT_MIN && v <= INT_MAX);
}

static inline int aos_check_u16(const u64 v)
{
	return (v <= USHRT_MAX);
}

static inline int aos_check_u32(const u64 v)
{
	return (v <= UINT_MAX);
}

static inline int aos_check_u8(const u64 v)
{
	return (v <= 255);
}

#endif


