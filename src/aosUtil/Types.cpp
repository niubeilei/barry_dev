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
#include "aosUtil/Types.h"

u64 AosStrToU64(const char *data)
{
	if (!data) return 0;
	char *pp;
	return strtoull(data, &pp, 10);
}

int64_t AosStrToInt64(const char *data)
{
	if (!data) return 0;
	char *pp;
	return strtoll(data, &pp, 10);
}

