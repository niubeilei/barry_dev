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
#include "util_c/strutil.h"

#include "alarm_c/alarm.h"
#include "KernelSimu/kernel.h"
#include "util_c/types.h"
#include "util_c/memory.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>


int AOS_MAX_STR_LENGTH = 100000;

int aos_a2bool(const char * const str, const int dft_value)
{
	aos_assert_r(str, dft_value);
	if (strcmp(str, "true") == 0) return 1;
	if (strcmp(str, "false") == 0) return 0;
	return dft_value;
}


// Added by Chen Ding, 02/28/2009
// This function checks the string contains only letters, 
// digits, and underscores, and it is not a pure digit string. 
// If yes, it returns 1. Otherwise, it returns 0. On errors, 
// it returns -1. 
int aos_str_is_varstr(const char *value)
{
	aos_assert_r(value, -1);
	int i;
	int len = strlen(value);
	int not_digit = 0;
	char c;
	for (i=0; i<len; i++)
	{
		c = value[i];
		if ((c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') || 
			c == '_')
		{
			if (c < '0' || c > '9') not_digit = 1;
		}
		else
		{
			return 0;
		}
	}

	return not_digit;
}	

// Added by Chen Ding, 07/29/2008
// This function splits the string 'orig_str' into a number of sub-strings, 
// separated by the character 'ch'. It allocates the memory as needed. The
// results are returned through 'reslts'. The caller should have allocated
// memory for 'results', as an array of pointers. The results_size indicates
// the size of 'results'. It can be used to control the splitting. When 
// this number is reached, the function will return immediately.
//
// Upon success, it returns the number of substrings found. Returning 1 means
// the character 'ch' was not found.
//
// If errors encountered, it returns -1.
//
int aos_str_split(const char *orig_str, 
		const char ch, 
		char **results, 
		const int results_size)
{
	aos_assert_r(orig_str, -1);
	int idx = 0;
	int start_idx = 0;
	char c;
	int found = 0;
	while ((c = orig_str[idx++]))
	{
		if (c == ch)
		{
			int len = idx - start_idx - 1; 
			char *sub = aos_malloc(len+1);
			aos_assert_r(sub, -1);
			memcpy(sub, &orig_str[start_idx], len);
			sub[len] = 0;
			results[found++] = sub;
			if (found >= results_size) return found;
			start_idx = idx;
		}
	}

	int len = idx - start_idx - 1;
	char *sub = aos_malloc(len+1);
	aos_assert_r(sub, -1);
	memcpy(sub, &orig_str[start_idx], len);
	sub[len] = 0;
	results[found++] = sub;
	if (found < results_size) results[found] = 0;
	return found;
}

// Added by Chen Ding, 12/14/2008
// The same as the above except that the separator is a string
//
int aos_sstr_split(const char *orig_str, 
		const char *sep, 
		char **results, 
		const int results_size)
{
	aos_assert_r(orig_str, -1);
	aos_assert_r(results, -1);
	aos_assert_r(sep, -1);
	aos_assert_r(strlen(sep) > 0, -1);
	aos_assert_r(results_size > 0, -1);


	int idx = 0;
	int start_idx = 0;
	char c;
	int found = 0;
	char ch = sep[0];
	int sep_len = strlen(sep);
	int i;
	for (i=0; i<results_size; i++) results[i] = 0;
	while ((c = orig_str[idx]))
	{
		if (c == ch && strncmp(&orig_str[idx], sep, sep_len) == 0)
		{
			int len = idx - start_idx; 
			char *sub = aos_malloc(len+1);
			aos_assert_r(sub, -1);
			memcpy(sub, &orig_str[start_idx], len);
			sub[len] = 0;
			results[found++] = sub;
			if (found >= results_size) return found;
			start_idx = idx + sep_len;
		}
		idx++;
	}

	int len = idx - start_idx;
	char *sub = aos_malloc(len+1);
	aos_assert_r(sub, -1);
	memcpy(sub, &orig_str[start_idx], len);
	sub[len] = 0;
	results[found++] = sub;
	if (found < results_size) results[found] = 0;
	return found;
}

int aos_str_split_releasemem(char **results, const int num)
{
	// This function releases the member in 'results', which is an array
	// of strings
	int i;
	for (i=0; i<num; i++)
	{
		if (!results[i]) return 0;

		aos_free(results[i]);
		results[i] = 0;
	}

	return 0;
}

// 
// The function copies the contents of 'rhs' to 'lhs'.
// This function is similar to strcpy. The difference is that
// it will allocate the memory for 'lhs' before copying the
// contents from 'rhs' to 'lhs'. It also checks whether 
// 'rhs' is too big. If it is too big, it will raise an alarm
// and abort the operation.
//
// Return Values:
// Success Code: 0. 
// Error Codes:
// 	1. When memory allocation failed
// 	2. When len is too big
int aos_str_set(char **lhs, const char * const rhs, const u32 len)
{
	aos_assert_r(lhs, -1);
	aos_assert_r(rhs, -1);
	aos_assert_r(len < AOS_MAX_STR_LENGTH, -1);

	*lhs = aos_malloc(len+1);
	aos_assert_r(*lhs, -1);
	strncpy(*lhs, rhs, len);
	(*lhs)[len] = 0;
	return 0;
}


// 
// Description:
// 	It copies rhs to lhs. If rhs is longer than max_len, it copies only
// 	max_len number of bytes. Otherwise, it copies all the contents from rhs
// 	to lhs. 
//
// IMPORTANT:
//  lhs must be at least max_len + 1 long. 
//
int aos_str_safe_copy(char *lhs, const char * const rhs, const u32 max_len)
{
	aos_assert_r(lhs, -1);
	aos_assert_r(rhs, -1);
	if (strlen(rhs) >= max_len)
	{
		strncpy(lhs, rhs, max_len);
	}
	else
	{
		strcpy(lhs, rhs);
	}

	return 0;
}


// 
// Description:
// 	It returns 1 if 'c' is in 'str' and 0 if 'c' is not in 'str'. 
// 	Negative integer is returned if anything is wrong.
//
int aos_str_in(const char * const str, 
			   const u32 len, 
			   const char c)
{
	int i;
	aos_assert_r(str, -1);
	for (i=0; i<len; i++)
	{
		if (c == str[i]) return 1;
	}

	return 0;
}


// 
// Description
// The function retrieves an integer from 'buff' at the position
// '*index'. It first skips any non-digital characters (['0'-'9', 
// '+' and '-' are considered digital characters). If successful, 
// 0 is returned. The value retrieved is stored in 'value'.
//
int aos_str_get_int(
		const char * const buff, 
		const int buff_len, 
		int *index, 
		int *value)
{
	char *endptr = 0;
	aos_assert_r(buff, -1);
	aos_assert_r(index, -1);
	aos_assert_r(value, -1);
	aos_assert_r(*index >= 0, -1);
	aos_assert_r(buff_len >= 0, -1);

	// Skip non-digit characters
	while (*index < buff_len)
	{
		char c = buff[(*index)];
		if ((c >= '0' && c <= '9') || c == '+' || c == '-') break;
		(*index)++;
	}

	aos_assert_r(*index < buff_len, -1);
	*value = strtol(&buff[*index], &endptr, 10);
	aos_assert_rm((unsigned long)endptr > (unsigned long)&buff[*index], -1, 
			"buff: %s, index: %d. bff_len: %ld", buff, *index, buff_len);
	*index = (unsigned long)endptr - (unsigned long)buff;
	return 0;
}


int aos_atoi_dft(const char * const str, const int dft_value)
{
	aos_assert_r(str, dft_value);
	char *endptr;
	int value = strtol(str, &endptr, 10);
	aos_assert_r(!(value == LONG_MIN && errno == ERANGE), -1);	// Check Underflow
	aos_assert_r(!(value == LONG_MAX && errno == ERANGE), -1);	// Check Overflow
	return value;
}


// 
// This function converts a string into an integer. It returns 0
// if and only if the entire string is consumed by the integer. 
// If the string is quoted, the quotes are removed before converting.
//
int aos_atoi(const char * const str, const int len, int *value)
{
	aos_assert_r(str, -1);
	aos_assert_r(value, -1);

	char *endptr;

	// Skip the white spaces
	int idx = 0;
	while (idx < len && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\n')) idx++;
	aos_assert_r(idx < len, -1);

	// Skip the quotes
	if (str[idx] == '"' || str[idx] == '\'') idx++;
	aos_assert_r(idx < len, -1);

	*value = strtol(&str[idx], &endptr, 10);

	aos_assert_r(!endptr || endptr[0] == 0 || endptr[0] == '"' || endptr[0] == '\'', -1);
	aos_assert_r(!(*value == LONG_MIN && errno == ERANGE), -1);	// Check Underflow
	aos_assert_r(!(*value == LONG_MAX && errno == ERANGE), -1);	// Check Overflow
	return 0;
}

// 
// This function converts a string into an unsigned integer. It returns 0
// if and only if the entire string is consumed by the integer. 
//
int aos_atou32(const char * const str, const int len, u32 *value)
{
	aos_assert_r(str, -1);
	aos_assert_r(value, -1);

	char *endptr;
	*value = (u32)strtoul(str, &endptr, 10);
	aos_assert_r(!endptr || endptr[0] == 0, -1);
	aos_assert_r(!(*value == ULONG_MAX && errno == ERANGE), -1);	// Check Overflow
	return 0;
}


// This function converts the string 'str' to an integer range [low, high]. 
// The two integers are separated by 'sep'. If successful, it returns 0. 
// Otherwise, it returns -1. Leading white spaces are skipped.
int aos_atoi_range(const char *const str, const char sep, int *low, int *high)
{
	aos_assert_r(str, -1);
	aos_assert_r(low, -1);
	aos_assert_r(high, -1);

	char *endptr;
	*low = strtol(str, &endptr, 10);
	aos_assert_r(str != endptr, -1);
	aos_assert_r(endptr, -1);
	aos_assert_r(endptr[0] == sep, -1);
	aos_assert_r(!(*low == LONG_MIN && errno == ERANGE), -1);	// Check Underflow
	aos_assert_r(!(*low == LONG_MAX && errno == ERANGE), -1);	// Check Overflow

	char *startptr = &endptr[1];
	aos_assert_r(startptr, -1);
	*high = strtol(startptr, &endptr, 10);
	aos_assert_r(!(*high == LONG_MIN && errno == ERANGE), -1);	// Check Underflow
	aos_assert_r(!(*high == LONG_MAX && errno == ERANGE), -1);	// Check Overflow
	if (!endptr) return 0;
	aos_assert_r(startptr != &endptr[1], -1);
	return 0;
}


#ifdef __KERNEL__
#include "linux/module.h"
#endif

int aosAsciiToBinary(char *from, char *to, unsigned int len)
{
	unsigned int i = 0;
	int index = 0;

	while (i < len)
	{
		if (from[index] >= '0' && from[index] <= '9')
		{
			to[i] = (from[index] - '0') << 4;
		}
		else if (from[index] >= 'a' && from[index] <= 'f')
		{
			to[i] = (from[index] - 'a') << 4;
		}
		else
		{
			return -1;
		}

		index++;
		if (from[index] >= '0' && from[index] <= '9')
		{
			to[i] += (from[index] - '0');
		}
		else if (from[index] >= 'a' && from[index] <= 'f')
		{
			to[i] += (from[index] - 'a');
		}
		else
		{
			return -1;
		}

		index++;
		i++;
	}

	return 0;
}


char * aosAddrToStr(u32 addr)
{
	static char lsName[20];
	sprintf(lsName, "%d.%d.%d.%d", 
		(unsigned int)(addr & 0xff),
		(unsigned int)((addr >> 8) & 0xff), 
		(unsigned int)((addr >> 16) & 0xff), 
		(unsigned int)((addr >> 24) & 0xff));
	return lsName;
}

static char sg_hex_map[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

// 
// The function converts the binary string 'input' into
// the form:
//	hhhh hhhh ...
// 
// where input[0] is converted into the first two h's 
// (hex representation of the byte value. 
// 
// Example:
//	input = 0x04, 0xa5, 0xfe, 0xcc, 0x12
//  output = "04a5 fecc 12"
//
// Parameters:
//	'input': IN
// 		The binary data.
//
//  'input_len': IN
//		The length of 'input'
//
//  'output': OUT
//		The output.
//	
//  'output_len': IN
//		It is the length of the 'output' buffer. 
//
// Return values:
//
//
int AosStrUtil_bin2HexStr(uint8_t *input, u32 input_len, 
						  uint8_t *output, u32 output_len)
{
	u32 index = 0;
	uint8_t ch;

	while (input_len-- && index + 2 < output_len)
	{
		ch = *input; input++;
	
		output[index++] = sg_hex_map[(ch >> 4) & 0x0f];	
		output[index++] = sg_hex_map[ch  & 0x0f];	
	}

	if (index + 2 >= output_len) return -1;

	output[index] = 0;

	return index;
}


// 
// This function treats the string 'str' as an integer. It
// increments the string by one. It first increment the last
// byte. If the last byte is 255, it sets the last byte to
// be 0 and increment the last second byte, and so on.
//
int AosStrUtil_binInc(uint8_t *str, u32 len)
{
	while (len > 0)
	{
		if (str[len-1] != 255)
		{
			str[len-1]++;
			return 0;
		}

		str[len-1] = 0;
		len--;
	}

	return 0;
}


//
//	This doesn't accept 0x if the radix is 16. The overflow code assumes
//	a 2's complement architecture
//
long AosStrToLong(char *string, char **endptr, int radix)
{
	char *s;
	long value;
	long new_value;
	int sign;
	int increment;

	value = 0;
	sign = 1;
	s = string;

	if ((radix == 1) || (radix > 36) || (radix < 0)) {
		goto done;
	}

	/* skip whitespace */
	while ((*s == ' ') || (*s == '\t') || (*s == '\n') || (*s == '\r')) {
		s++;
	}

	if (*s == '-') {
		sign = -1;
		s++;
	} else if (*s == '+') {
		s++;
	}

	if (radix == 0) {
		if (*s == '0') {
			s++;
			if ((*s == 'x') || (*s == 'X')) {
				s++;
				radix = 16;
			} else
				radix = 8;
		} else
			radix = 10;
	}

	/* read number */
	while (1) {
		if ((*s >= '0') && (*s <= '9'))
			increment = *s - '0';
		else if ((*s >= 'a') && (*s <= 'z'))
			increment = *s - 'a' + 10;
		else if ((*s >= 'A') && (*s <= 'Z'))
			increment = *s - 'A' + 10;
		else
			break;

		if (increment >= radix)
			break;

		new_value = value * radix + increment;
		// detect overflow 
		if ((new_value - increment) / radix != value) {
			s = string;
			value = -1 >> 1;
			if (sign < 0)
				value += 1;

			goto done;
		}

		value = new_value;
		s++;
	}

done:
	if (endptr)
		*endptr = s;

	return value * sign;
}
/*
//
// it will check th input format 
// the format should be like 192.168.0.1
// 
// return 0; failed
// 
unsigned long AosAddrStrToLong( char* ip_input )
{
	typedef enum{
		IP_ADDR_STATUS_IP1 = 0,
		IP_ADDR_STATUS_IP1_DOT,
		IP_ADDR_STATUS_IP2,
		IP_ADDR_STATUS_IP2_DOT,
		IP_ADDR_STATUS_IP3,
		IP_ADDR_STATUS_IP3_DOT,
		IP_ADDR_STATUS_IP4,
		IP_ADDR_STATUS_END,
	};

	uint8_t ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
	char *pos;
	int len = strlen(pos);
	int index, format_right = 0;
	
	pos = ip_input;
	while( *pos )
	{
		if( *pos >= '0' && *pos <= '9' )
		{
			switch( state )
			{
				case IP_ADDR_STATUS_IP1:
					ip1 = ip1*10 + *pos - '0';
					break;
				case IP_ADDR_STATUS_IP1_DOT:
					state++;
					break;
				case IP_ADDR_STATUS_IP2:
					ip2 = ip2*10 + *pos - '0';
					break;
				case IP_ADDR_STATUS_IP2_DOT:
					state++;
					break;
				case IP_ADDR_STATUS_IP3:
					ip3 = ip3*10 + *pos - '0';
					break;
				case IP_ADDR_STATUS_IP3_DOT:
					state++;
					break;
				case IP_ADDR_STATUS_IP4:
					ip4 = ip4*10 + *pos - '0';
					format_right = 1;
					break;
				default:
					return -1;
			}
		}// if( *pos < '0' && *pos < '9' )
		else if( *pos == '.' )
		{
			switch( state )
			{
				case IP_ADDR_STATUS_IP1:
				case IP_ADDR_STATUS_IP2:
				case IP_ADDR_STATUS_IP3:
					state++;
					break;
				default:
					return 0;
			}
				
		}
		else
		{
			return 0;
		}
		pos++;
	}// while( *pos )

	
	if ( !format_right ) return 0

	if ( ip1 > 255 || ip2 > 255 || ip3 > 255 || ip4 > 255 ) return 0;

	return ( (ip4<<24)|(ip3<<16)|(ip2<<8)|ip1 );
}

//
// convert ip address to binary string which is 32 bytes long
// for example 192.168.0.1 will be
// 1 1 0 0 0 0 0 0  1 0 0 1 1 0 0 0  0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 1
// 
int AosLongToBinStr( unsigned long ip_addr, char* ip_keyword_out, int out_len )
{
	int i;
	if ( out_len < 33 ) return -1;
	for( i = 0; i < 32; i++ )
	{
		ip_keyword_out[i] = ip_addr & ( 0x1 << (31 - i ) );
	}
	ip_keyword_out[i] = 0;
}
*/

#ifdef __KERNEL__
EXPORT_SYMBOL( aosAddrToStr );
EXPORT_SYMBOL( AosStrToLong );
#endif
