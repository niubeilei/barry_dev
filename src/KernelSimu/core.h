////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: core.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_core_h
#define Omn_aos_KernelSimu_core_h

#ifdef AOS_KERNEL_SIMULATE

#ifndef kmalloc
#define kmalloc(size, flag) malloc(size)
#endif

#ifndef kfree
#define kfree(x) free(x)
#endif

#else
// #include <linux/core.h>
#endif

#endif

