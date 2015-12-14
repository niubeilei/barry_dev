////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: System.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_System_h
#define Aos_AosUtil_System_h

#include "aosUtil/Types.h"

#ifndef AOS_POINTER_POISON
#define AOS_POINTER_POISON 0x12349876
#endif

#ifndef AOS_INT_POISON
#define AOS_INT_POISON     0x22336644
#endif

#ifndef AOS_U8_POISON
#define AOS_U8_POISON     0x22
#endif

static inline const char *AosSystem_getDataDir(void)
{
	return "/usr/local/AOS/Data";
}

#endif

