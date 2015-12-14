////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: udp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_udp_h
#define Omn_aos_KernelSimu_udp_h

#ifdef AOS_KERNEL_SIMULATE

#include <KernelSimu/types.h>


struct udphdr {
	__u16	source;
	__u16	dest;
	__u16	len;
	__u16	check;
};


#else
#include <linux/udp.h>
#endif

#endif

