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
//
// Modification History:
// 02/02/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/valconv.h"

#include "alarm_c/alarm.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <errno.h>



// 
// Description:
// It converts string/binrary into a long long. It returns 0 if and only if 
// the entire contents can be correctly converted into a long long. 
//
int aos_value_atoll(const char * const str, int64_t *vv)
{
	aos_assert_r(str, -1);
	aos_assert_r(vv, -1);
	
	// The string must be a digit string. It may start with a
	// sign character. 
	char *endptr;
	*vv = strtoll(str, &endptr, 10);

	// Make sure it consumed some characters
	aos_assert_rm(str != endptr, -1, "Contents: %s", str);

	// If errno == ERANGE and *vv is either LLONG_MIN or LLONG_MAX, it indicates
	// either underflow (LLONG_MIN) or overflow (LLONG_MAX).
	aos_assert_r(!((*vv == LLONG_MIN|| *vv == LLONG_MAX) && errno == ERANGE), -1);
/*
	// Make sure the remaining characters are white spaces
	int idx = (int)endptr - (int)str;
	int len = strlen(str);
	for (; idx < len; idx++)
	{
		aos_assert_rm(str[idx] == ' ' ||
					 str[idx] == 0 ||
					 str[idx] == '\t' ||
					 str[idx] == '\r' ||
					 str[idx] == '\n', -1, 
					 "Char: %d", str[idx]);
	}
*/
	return 0;
}


// 
// Description:
// It converts string/binrary into an unsigned long long. It returns 0 if and only if 
// the entire contents can be correctly converted. 
//
int aos_value_atoull(const char * const str, u64 *vv)
{
	aos_assert_r(str, -1);
	aos_assert_r(vv, -1);
	
	// The string must be a digit string. It may start with a
	// sign character. 
	char *endptr;
	*vv = strtoull(str, &endptr, 10);

	// Make sure it consumed some characters
if (str == endptr)
	printf("dd");
	aos_assert_r(str != endptr, -1);

	// If errno == ERANGE and *vv is ULLONG_MAX, it indicates
	// overflow.
	aos_assert_r(!(*vv == ULLONG_MAX && errno == ERANGE), -1);

	// Make sure the remaining characters are white spaces
	/*
	int idx = (int)endptr - (int)str;
	int len = strlen(str);
	for (; idx < len; idx++)
	{
		aos_assert_rm(str[idx] == ' ' ||
					 str[idx] == 0 ||
					 str[idx] == '\t' ||
					 str[idx] == '\r' ||
					 str[idx] == '\n', -1, 
					 "Char: %d", str[idx]);
	}
	*/

	return 0;
}


