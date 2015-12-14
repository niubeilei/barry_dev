////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aosUtil_Random_h
#define aos_aosUtil_Random_h

#include "aosUtil/Types.h"


extern void AosRandom_get(char *buff, u32 bufflen);

#ifdef __KERNEL__

#include <linux/net.h>

#ifndef aos_random
#define aos_random net_random
#endif

#elif OMN_PLATFORM_UNIX

#include <stdlib.h>

#ifndef aos_random
#define aos_random rand
#endif

#endif

#endif

