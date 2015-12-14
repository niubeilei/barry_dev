////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelStr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_KernelStr_h
#define Omn_KernelUtil_KernelStr_h

#include "aos/aosKernelAlarm.h"

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "aosUtil/Types.h"

/*
static inline int aosCheckAndCopy(char *result, 
						   unsigned int *index, 
						   const unsigned int length, 
						   const char *buf, 
						   const unsigned int len)
{
	if (!result || !index || !buf || 
		len >= length - *index - 1 ||
		*index < 0 || *index >= length)
	{
		return aosAlarm4(eAosAlarmProgErr, length, *index, result, buf);
	}

	strncpy(&result[*index], buf, len);
	*index += len;
	result[*index] = 0;
	return 0;
}
*/

extern int aosAsciiToBinary(char *from, char *to, unsigned int len);
extern char *aosAddrToStr(u32 addr);

#endif

