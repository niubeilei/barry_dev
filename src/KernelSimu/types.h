////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: types.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_types_h
#define Omn_aos_KernelSimu_types_h

#ifdef AOS_KERNEL_SIMULATE

// #include "Porting/LongTypes.h"
#include "aosUtil/Types.h"


#ifdef OMN_PLATFORM_UNIX
#include <asm/types.h>
#endif

// typedef unsigned char		u8;
// typedef OmnUint64 			u64;
// typedef unsigned short 		u16;
// typedef unsigned long 		u32;

/*
typedef unsigned int		__u32;
typedef long 		  		__s32;
typedef unsigned char 		__u8;
typedef unsigned short		__u16;
*/

#define __constant_htons(x) (x)

#ifdef OMN_PLATFORM_MICROSOFT
typedef unsigned int		__u32;
typedef long 		  		__s32;
typedef unsigned char 		__u8;
typedef unsigned short		__u16;
typedef unsigned int		ssize_t;
#endif


#else
#include <linux/types.h>
#endif

#endif

