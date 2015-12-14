////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Debug.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Debug_h
#define Aos_AosUtil_Debug_h

#include "aosUtil/Types.h"
#include "aosUtil/FuncEnum.h"


enum
{
	eAosDebug_FuncTraceEntries = 0x07ff,
};

extern u32 aos_func_trace[eAosDebug_FuncTraceEntries];
extern u32 aos_func_trace_index;


	
#ifndef AOS_NO_FUNC_TRACK

#ifndef aos_enter_func
#define aos_enter_func(x) 		\
	aos_func_trace[aos_func_trace_index++ & eAosDebug_FuncTraceEntries] = x;
#endif

#endif

#endif

