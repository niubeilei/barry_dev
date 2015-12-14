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
#include "semantics_c/memory.h"

#include "alarm_c/alarm.h"
#include <string.h>

char * aos_mat_to_str(const aos_mat_t code)
{
	switch (code)
	{
	case eAosMAT_Unknown:		return "unknown";
	case eAosMAT_Invalid: 		return "invalid";
	case eAosMAT_Stack:			return "stack";
	case eAosMAT_Heap:			return "heap";
	case eAosMAT_Global:		return "global";
	case eAosMAT_StaticGlobal:	return "static_global";
	case eAosMAT_StaticLocal:	return "static_local";
	default:
		 aos_alarm("Unrecognized memory allocation type: %d\n", code);
		 return "unknown";
	}

	aos_should_never_come_here;
	return "program_error";
}

	
aos_mat_t aos_mat_to_enum(const char * const name)
{
	aos_assert_r(name, eAosMAT_Unknown);
	if (strcmp(name, "invalid") == 0)		return eAosMAT_Invalid;
	if (strcmp(name, "stack") == 0)			return eAosMAT_Stack;
	if (strcmp(name, "heap") == 0)			return eAosMAT_Heap;
	if (strcmp(name, "global") == 0)		return eAosMAT_Global;
	if (strcmp(name, "static_global") == 0)	return eAosMAT_StaticGlobal;
	if (strcmp(name, "static_local") == 0)	return eAosMAT_StaticLocal;

	aos_alarm("Unrecognized memory allocation type: %s\n", name);
	return eAosMAT_Unknown;
}


u32 aos_check_malloc_max_size(const u32 size)
{
	if (size > AOS_MALLOC_MAX_SIZE)
	{
		aos_alarm("Trying to malloc a memory that is too big: %d", size);
	}
	return size;
}

