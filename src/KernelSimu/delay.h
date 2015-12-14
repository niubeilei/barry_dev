////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: delay.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_aos_KernelSimu_delay_h
#define Omn_aos_KernelSimu_delay_h

#ifndef __KERNEL__

#include <unistd.h>

void msleep(unsigned int msecs)
{
	usleep(ms * 1000);
}

static inline void ssleep(unsigned int ms)
{
	usleep(ms * 1000);
}

#else
#include <linux/delay.h>
#endif

#endif

