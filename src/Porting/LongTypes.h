////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LongTypes.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_LongTypes_h
#define Omn_Porting_LongTypes_h

#ifndef aos_ll_hashkey
#define aos_ll_hashkey(x) \
	(unsigned int)((x& 0xff) + 		\
              ((x >> 8)  & 0xff) +	\
              ((x >> 16) & 0xff) +	\
              ((x >> 24) & 0xff) +	\
              ((x >> 32) & 0xff) +	\
              ((x >> 40) & 0xff) +	\
              ((x >> 48) & 0xff) +	\
              ((x >> 56) & 0xff))
#endif

#ifdef OMN_PLATFORM_UNIX


#ifndef aos_atoll
#define aos_atoll(str) atoll(str)
#endif

#ifndef aos_atoull
#define aos_atoull(str) (unsigned long long)atoll(str)
#endif

#ifndef aos_lltoa
#define aos_lltoa(value, str) sprintf(str, "%lld", (long long int)value)
#endif
 
#ifndef aos_ulltoa
#define aos_ulltoa(value, str) sprintf(str, "%ld", value)
#endif
 
#else
#ifdef OMN_PLATFORM_MICROSOFT

#ifndef aos_atoll
#define aos_atoll(str) _atoi64(str)
#endif

#ifndef aos_atoull
#define aos_atoull(str) (unsigned __int64)_atoi64(str)
#endif

#ifndef aos_lltoa
#define aos_lltoa(value, str) sprintf(str, "%I64i", value)
#endif

#ifndef aos_ulltoa
#define aos_ulltoa(value, str) sprintf(str, "%I64u", value)
#endif

#endif
#endif

// 
// Define OmnInt64, OmnUint64
//
#ifdef OMN_PLATFORM_MICROSOFT

#define OmnInt64		__int64
#define OmnUint64		unsigned __int64

#else

#ifdef __KERNEL__
#include <linux/types.h>
#define OmnInt64	__int64
#define OmnUint64	__u64
#else
#include </usr/include/stdint.h>
#define OmnInt64	int64_t
#define OmnUint64	uint64_t
#endif

#endif


#ifdef AOS_USERLAND
//john, if include stdlib.h, qos can not be compiled on userland
//#include <stdlib.h>
#include <stdio.h>

#endif
#endif

