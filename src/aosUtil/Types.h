////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file defines the commonly used data types. Below are the standard
// types we should be using:
// 		int8_t
//		int16_t
//		int32_t
//		int64_t
//		uint8_t
//		uint16_t
//		uint32_t
//		uint64_t
//
// Below are the abbrieviation form of unsigned integers:
// 		u8
// 		u16
// 		u32
// 		u64
//
// Modification History:
// 11/26/2007: Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosUtil_Types_h
#define Aos_AosUtil_Types_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifdef __KERNEL__
#include <linux/types.h>
#endif


typedef uint8_t 				u8;
typedef uint16_t				u16;
typedef uint32_t				u32;
typedef uint64_t				u64;

typedef int8_t 					i8;
typedef int16_t					i16;
typedef int32_t					i32;
typedef int64_t					i64;

typedef float					d32;
typedef double					d64;

// Chen Ding, 2013/02/09
#define AOS_CONST_MB 1000 * 1000ULL
#define AOS_CONST_GB 1000 * 1000 * 1000ULL

#ifdef __amd64__
#else
#endif

#ifndef LLONG_MAX
#define LLONG_MAX    9223372036854775807LL
#endif

#ifndef LLONG_MIN
#define LLONG_MIN    (-LLONG_MAX - 1LL)
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX   18446744073709551615ULL
#endif

#define __constant_htons(x) (x)

extern u64 AosStrToU64(const char *data);
extern int64_t AosStrToInt64(const char *data);

#endif


