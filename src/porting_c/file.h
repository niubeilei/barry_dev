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
// 01/30/2008: Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_porting_file_h
#define aos_porting_file_h


#ifdef __cplusplus
extern "C" {
#endif

#ifdef OMN_PLATFORM_UNIX
#include "util_c/types.h"

extern int64_t aos_file_get_size(const char * const filename);


#elif defined(OMN_PLATFORM_MICROSOFT)
// 
// Microsoft Portion
//
#endif 	// Platform

#ifdef __cplusplus
}
#endif

#endif	// File


