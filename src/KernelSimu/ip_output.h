////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ip_output.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_ip_output_h
#define Omn_aos_KernelSimu_ip_output_h

#ifdef AOS_KERNEL_SIMULATE

extern void ip_send_check(struct iphdr *iph);

#else
#include <linux/ip_output.h>
#endif

#endif

