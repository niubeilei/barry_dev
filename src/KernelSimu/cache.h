////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cache.h 
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_cache_h
#define Omn_aos_KernelSimu_cache_h

#ifdef __KERNEL__
#include <linux/cache.h>

#else

// Userland Portion
#ifndef SMP_CACHE_BYTES
#define SMP_CACHE_BYTES 16
#endif

#endif

#endif //#ifndef Omn_aos_KernelSimu_cache_h


