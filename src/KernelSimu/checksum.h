////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: checksum.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_checksum_h
#define Omn_aos_KernelSimu_checusum_h

#ifdef AOS_KERNEL_SIMULATE

inline unsigned int csum_partial_copy_generic(const unsigned char *src, 
	unsigned char *dst,
    int len, int sum, int *src_err_ptr, int *dst_err_ptr)
{
	return 0;
}

static inline unsigned short int csum_tcpudp_magic(unsigned long saddr,
						   unsigned long daddr,
						   unsigned short len,
						   unsigned short proto,
						   unsigned int sum)
{
	return 0;
}

static inline unsigned int
csum_partial(const unsigned char * buff, int len, unsigned int sum)
{
	return 0;
}

static inline unsigned int csum_add(unsigned int csum, unsigned int addend)
{
	csum += addend;
	return csum + (csum < addend);
}


#else
#include <asm/checksum.h>
#endif

#endif

