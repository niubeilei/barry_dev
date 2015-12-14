////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelDebug.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelSimu_KernelDebug_h
#define aos_KernelSimu_KernelDebug_h

#ifdef AOS_KERNEL_SIMULATE

#include <stdio.h>

#ifndef printk
#define printk printf
#endif

#else


#endif

#endif

