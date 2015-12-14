////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sched.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_sched_h
#define Omn_aos_KernelSimu_sched_h

#ifdef AOS_KERNEL_SIMULATE

static inline int capable(int cap)
{
	return 1;
}


#else
#include <linux/sched.h>
#endif

#endif

