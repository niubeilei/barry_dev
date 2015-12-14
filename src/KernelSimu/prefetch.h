////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: prefetch.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_prefetch_h
#define Omn_aos_KernelSimu_prefetch_h

#ifdef __KERNEL__
#include <linux/prefetch.h>
#else
static inline void prefetch(const void *x) {;}
#endif

#endif

