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
//  This file contains a group of random functions and a random class. 
//  The OmnRandom class is obselete. You are encouraged to use the 
//  random functions.  
//
// Modification History:
// 01/25/2007 Created by Chen Ding
// 05/18/2007 Moved from Util by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/random_util.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomStr.h"
#include "Util/IpAddr.h"
#include <arpa/inet.h>

bool aos_next_pct(const u32 pct)
{
	return ((u32)aos_next_int(1, 100) <= pct);
}


u32 aos_next_u32(const u32 min, const u32 max)
{
	if (min > max)
	{
		OmnAlarm << "min > max: " << min << ":" << max << enderr;
		return min;
	}

	u32 space = max - min;
	if (space == 0) return min;
	return rand() % space + min;
}


int aos_next_int(const int min, const int max)
{
	if (min > max)
	{
		OmnAlarm << "min > max: " << min << ":" << max << enderr;
		return min;
	}

	unsigned int space = abs(max - min) +1;

	return rand()%space + min;
}


int64_t aos_next_int64(const int64_t min, const int64_t max)
{
	if (min > max)
	{
		OmnAlarm << "min > max: " << min << ":" << max << enderr;
		return min;
	}

	int64_t space = (max - min) +1;

	return rand()%space + min;
}


/*
OmnString aos_get_ip_byte()
{
	// 
	// An IP byte is integer in the range of [0, 255]. 
	// This function will randomly determine whether to generate
	// a correct one or incorrect one. If it is incorrect, 
	// we may make it incorrect in the following ways:
	// 1. negative integers
	// 2. integers larger than 255
	// 3. not a digit string
	//
	static int ls_max_ip_str_len = 5;
	static int ls_large_ip_byte_value = 1000;
	static int ls_min_negative = -1000;
	static int ls_max_negative = -1;

	OmnString str;
	if (aos_next_pct(create_correct_ip_byte_pct))
	{
		str << aos_next_int(0, 255);
		return str;
	}

	switch (aos_next_int(0, 2))
	{
	case 0:
		 // 
		 // Generate a negative integer
		 //
		 str << aos_next_int(ls_min_negative, ls_max_negative);
		 return str;

	case 1:
		 str << aos_next_int(256, ls_large_ip_byte_value);
		 return str;

	case 2:
		 str << aos_next_printable_str(1, ls_max_ip_str_len);
		 return str;
	}

	OmnAlarm << "Program error" << enderr;
	return str;
}
*/


OmnString aos_next_incorrect_ip()
{
	OmnNotImplementedYet;
	return "";
	/*
	// 
	// We will generate an incorrect IP based on the following:
	// 1. Totally wrong (just a random string)
	// 2. Wrong number of dots
	// 3. Wrong sub-strings between dots
	//
	if (aos_next_pct(totally_incorrect_ip_pct))
	{
		u32 len = aos_next_int(1, 20);
		OmnString str = aos_next_str(eAosStrType_VarName, 1, len);
		if (OmnIpAddr::isValidAddr(str))
		{
			// 
			// VERY VERY UNLIKELY. 
			//
			return "12345_VERY_UNLIKELY_67890";
		}

		return str;
	}


	int tries = 5;
	while (tries--)
	{
		int numDots = aos_next_int(0, 3);
		OmnString str = aos_get_ip_byte();
		for (int i=0; i<numDots; i++)
		{
			str << "." << aos_get_ip_byte();
		}
	
		// 
		// Check whether we want to add a trailing dot
		//
		if (aos_next_pct(add_trailing_dot_pct))
		{
			str << ".";
		}

		if (!OmnIpAddr::isValidAddr(str))
		{
			return str;
		}
	}

	// 
	// VERY VERY UNLIKELY. 
	//
	return "12345_VERY_UNLIKELY_67890";
*/
}


OmnIpAddr
aos_next_ip(const OmnIpAddr &netaddr, const OmnIpAddr &netmask)
{
	u32 n = netaddr.getIPv4();
	u32 m = netmask.getIPv4();
	u32 rr = htonl(n) & htonl(m);
	u32 max = htonl(~m);
	return OmnIpAddr(htonl(aos_next_u32(0, max) + rr));
}


OmnIpAddr
aos_next_mask()
{
	return aos_next_mask(0, 32);
}


OmnIpAddr
aos_next_mask(const u32 min, const u32 max)
{
	// 
	// It randomly generates a mask that has at least 'min' 1's
	// and at most 'max' 1's. 
	//
	int numOnes = aos_next_int(min, max);
	u32 addr = (numOnes)?(0xffffffff << (32 - numOnes)):0;
	return OmnIpAddr(htonl(addr));
}


OmnString
aos_next_mask_str(const u32 min, const u32 max)
{
	return aos_next_mask(min, max).toString();
}


OmnString
aos_next_ip_str(const OmnIpAddr &netaddr, const OmnIpAddr &netmask)
{
	return aos_next_ip(netaddr, netmask).toString();
}


OmnIpAddr 
aos_next_bad_mask()
{
	// 
	// It randomly generates a bad mask. A correct net mask
	// is a four byte string in the form of "1111...00000". 
	// An incorrect net mask is an integer that is not in 
	// the form of "1111...0000".
	//
	int tries = 5;
	while (tries--)
	{
		OmnIpAddr addr(aos_next_u32(1, 0xffffffff));
		if (!OmnIpAddr::isValidMask(addr))
		{
			return addr;
		}
	}

	//
	// This should RARELY, RARELY happen
	//
	return OmnIpAddr(12345);
}


OmnString aos_next_time_str(AosTimeStrFormat_e format)
{
	// 
	// This will generate a time string in the format of:
	// 		HH:MM:SS
	// 		HH:MM
	// 		HH
	// where:
	// 	HH can be one or two digits, representing the hour [0-23]
	// 	MM can be one or two digits, representing the minutes [0-59]
	// 	SS can be one or two digits, representing the seconds [0-59]
	//
	OmnString str;
	char buff[100];

	if (aos_next_pct(50))
	{
		sprintf(buff, "%.2d", aos_next_int(0, 23));
		str << buff;
	}
	else
	{
		str << aos_next_int(0, 23);
	}
	if (format == eAosTimeStrFormat_HH) return str;

	if (aos_next_pct(50))
	{
		sprintf(buff, ":%.2d", aos_next_int(0, 59));
		str << buff;
	}
	else
	{
		str << ":" << aos_next_int(0, 59);
	}
	if (format == eAosTimeStrFormat_HHMM) return str;

	if (aos_next_pct(50))
	{
		sprintf(buff, ":%.2d", aos_next_int(0, 59));
		str << buff;
	}
	else
	{
		str << ":" << aos_next_int(0, 59);
	}
	return str;
}


OmnString aos_next_invalid_time_str(const u32 min, const u32 max)
{
	// 
	// It generates invalid time string.
	//
	aos_assert_r(min <= max, "**BAD**");
	if (min == max && min == 0) return "";

	OmnString str;
	if (aos_next_pct(90))
	{
		char buff[10];
		sprintf(buff, "%.2d:", aos_next_int(0, 23));
		str << buff;
	}
	else
	{
		str << OmnRandom::noSpaceStr(min, max);
	}

	if (aos_next_pct(90))
	{
		char buff[10];
		sprintf(buff, "%.2d:", aos_next_int(0, 59));
		str << buff;
	}
	else
	{
		str << OmnRandom::noSpaceStr(min, max);
	}

	if (aos_next_pct(90))
	{
	 	char buff[10];
		sprintf(buff, "%.2d", aos_next_int(0, 59));
		str << buff;
	}
	else
	{
	 	str << OmnRandom::noSpaceStr(min, max);
	}

	// 
	// Make sure it is an incorrect string
	//
	aos_assert_r(str.length() > 0, "**BAD**");
	char *buff = (char *)str.data();
	int index = aos_next_int(0, str.length()-1);
	buff[index] = (char)aos_next_int('a', 'z');

	return str;
}


OmnMacAddr
aos_next_mac()
{
	return OmnRandom::netMac();
}


char aos_next_hex_char()
{
	// 
	// A hex character is [0-9, a-f, A-F].
	//
	int pct = aos_next_int(1, 100);
	if (pct < 33) return '0' + aos_next_int(0, 9);
	if (pct < 66) return 'a' + aos_next_int(0, 5);
	return 'A' + aos_next_int(0, 5);
}


OmnString aos_next_hex_byte()
{
	// 
	// A hex byte is "xx", where 'x' is [0-9, a-f, A-F]. 
	// It may be one or two characters in the string.
	//
	char data[3];	
	if (aos_next_pct(50))
	{
		data[0] = aos_next_hex_char();
		data[1] = 0;
		return data;
	}

	data[0] = aos_next_hex_char();
	data[1] = aos_next_hex_char();
	data[2] = 0;
	return data;
}


OmnString aos_next_incorrect_mac()
{
	OmnString str;
	for (int i=0; i<6; i++)
	{
		if (aos_next_pct(90))
		{
			str << aos_next_hex_byte();
		}
		else
		{
			str << (char)aos_next_int(33, 126);
		}
			
		if (i < 5)
		{
			if (aos_next_pct(90))
			{
				str << ":";
			}
			else
			{
				str << (char)aos_next_int(33, 126);
			}
		}
	}

	char *data = (char *)str.data();
	int index = aos_next_int(0, str.length()-1);
	data[index] = 'x';
	return str;
}


