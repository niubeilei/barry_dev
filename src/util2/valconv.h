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
// 02/02/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util2_valconv_h
#define aos_util2_valconv_h

#include "util_c/types.h"
#include <stdio.h>


extern int aos_value_atoll(const char * const str, int64_t *vv);
extern int aos_value_atoull(const char * const str, u64 *vv);
inline int aos_value_atoul(const char * const str, u32 *vv)
{
	u64 v;
	int n = aos_value_atoull(str, &v);
	if (*vv > 0xffffffff)
	{
		printf("********** %s:%d: unsigned long too big: %s\n", 
			__FILE__ , __LINE__, str); 
	}
	*vv = (u32)v;
	return n;
}
#endif

