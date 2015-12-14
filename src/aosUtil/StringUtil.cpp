////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringUtil.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/StringUtil.h"

#include "aosUtil/Types.h"
#include "aosUtil/ReturnCode.h"
#include "KernelSimu/kernel.h"

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
			return eAosRc_IncorrectHexAscii;
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
			return eAosRc_IncorrectHexAscii;
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

	if (index + 2 >= output_len) return -eAosRc_StrUtilErr;

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
