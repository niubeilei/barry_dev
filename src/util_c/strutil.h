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
//
// Modification History:
// 01/17/2008: Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_util_strutil_h
#define aos_util_strutil_h

#include "alarm_c/alarm.h"
#include "util_c/types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
// 
// !!!!!!!!!!! No strings can be longer than 100M!!!!!!!!!!!!
//
#define aos_str_len_assert_r(len, rc)			\
	if (len < 0 || len > 100000000)				\
	{											\
		aos_alarm("String too long: %d", len);	\
		return (rc);							\
	}

#define aos_str_len_assert(len)					\
	if (len < 0 || len > 100000000)				\
	{											\
		aos_alarm("String too long: %d", len);	\
		return;									\
	}

extern char * aos_str_int_to_str(const int64_t value);
extern int aos_str_set(char **lhs, const char * const rhs, const u32);
extern int aos_str_safe_copy(char *lhs, const char * const rhs, const u32 max_len);
extern int aos_str_in(const char * const str, const u32 len, const char c);
extern int aos_str_get_int(
		const char * const buff, 
		const int buff_len, 
		int *index, 
		int *value);
extern int aos_a2bool(const char * const str, const int dft_value);
extern int aos_atoi_dft(const char * const str, const int dft_value);
extern int aos_atoi(const char * const str, const int len, int *value);
extern int aos_atou32(const char * const str, const int len, u32 *value);
extern int aos_str_split(const char *orig_str, const char ch, char **results, const int results_size);
extern int aos_sstr_split(const char *orig_str, const char *ch, char **results, const int results_size);
extern int aos_str_split_releasemem(char **results, const int num);
extern int aos_str_is_varstr(const char *value);
int aos_atoi_range(const char *const str, const char sep, int *low, int *high);

#ifdef __cplusplus
}
#endif


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

		return -1;
	}

	if (strlen(name) >= maxLen)
	{
		sprintf(errmsg, "%s too long: %zd. Max: %d", errName, strlen(name), maxLen);
		return -1;
	}

	return 0;
}

extern int AosStrUtil_binInc(uint8_t *str, u32 len);

extern long AosStrToLong(char *string, char **endptr, int radix);
#endif

