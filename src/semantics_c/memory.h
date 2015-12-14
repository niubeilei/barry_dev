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
// 12/15/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semantics_memory_h
#define aos_semantics_memory_h

#include "util_c/types.h"

// Memory Allocation Type (MAT)
typedef enum
{
	eAosMAT_Unknown, 
	eAosMAT_Invalid, 

	eAosMAT_Stack, 
	eAosMAT_Heap, 
	eAosMAT_Global, 
	eAosMAT_StaticGlobal, 
	eAosMAT_StaticLocal
} aos_mat_t;

extern char * 		aos_mat_to_str(const aos_mat_t code);
extern aos_mat_t 	aos_mat_to_enum(const char * const name);

// Maximum malloc size is 1G
#define AOS_MALLOC_MAX_SIZE 1000000000

extern inline u32 aos_check_malloc_max_size(const u32 size);
	

#endif // End of Include

