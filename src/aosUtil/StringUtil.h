////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringUtil.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_StringUtil_h
#define Aos_AosUtil_StringUtil_h

#include "aos/aosReturnCode.h"
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include <stdio.h>

#ifdef __KERNEL__
#include <linux/string.h>
extern int sprintf(char * buf, const char *fmt, ...);
#else
#include <string.h>
#endif


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
		aos_alarm(
			"Length=%d, Index=%lx, Reslt=%lx, buf=%lx, *Index=%d, len=%d", 
			length, (unsigned long)index,(unsigned long)result, 
			(unsigned long)buf,*index, len);
		return -1;
	}

	strncpy(&result[*index], buf, len);
	*index += len;
	result[*index] = 0;
	return 0;
}


extern int AosStrUtil_bin2HexStr(uint8_t *input, u32 input_len, 
						  uint8_t *output, u32 output_len);
extern int aosAsciiToBinary(char *from, char *to, unsigned int len);
extern char *aosAddrToStr(u32 addr);
static inline int aosStrUtil_checkName(const char *name, 
								const unsigned int maxLen,
								const char *errName, 
								char *errmsg)
{
	if (!name) 
	{
		if (errmsg)
		{
			sprintf(errmsg, "%s is null", errName);
		}

		return -eAosRc_NullPointer;
	}

	if (strlen(name) >= maxLen)
	{
		sprintf(errmsg, "%s too long: %zu. Max: %u", errName, strlen(name), maxLen);
		//sprintf(errmsg, " too long: %lu. Max: ", strlen(name));
		return -eAosRc_NameTooLong;
	}

	return 0;
}

extern int AosStrUtil_binInc(uint8_t *str, u32 len);

extern long AosStrToLong(char *string, char **endptr, int radix);
#endif

