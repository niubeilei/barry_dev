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
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_porting_addr_h
#define Omn_porting_addr_h

//////////////////////  MICROSOFT  ////////////////////////////////////
#ifdef OMN_PLATFORM_MICROSOFT

#include <winsock2.h>

static inline int aos_addr_to_u32(
		const char *ipAddrDotNotation, 
		u32 *addr)
{
	//
	// The function: inet_addr(...) from Microsoft converts the IPv4
	// dot notation into a long, in the network order. 
	//
	aos_assert_r(ipAddrDotNotation, -1);
	aos_assert_r(addr, -1);
	*addr = inet_addr(ipAddrDotNotation);
	return 0;
}


inline char * aos_addr_to_str(const u32 addr)
{
	//
	// It returns the dots notation of the ip address addr.
	//
	in_addr inp;
	inp.S_un.S_addr = hton(addr);
	return inet_ntoa(inp);
}


//////////////////////  Linux       ////////////////////////////////////
#elif __KERNEL__
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
inline int aos_addr_to_u32(
		const char *ipAddrDotNotation, 
		u32 *addr)
{
	//
	// If the input address is invalid, it returns zero;
	// otherwise, it returns the binary  data in network byte order.
	//
	aos_assert_r(ip_addrDotNotation, -1);
	aos_assert_r(addr, -1);
	*addr = in_aton(ipAddrDotNotation);
	return 0;
}


#elif OMN_PLATFORM_UNIX
#include "alarm_c/alarm.h"
#include "util_c/types.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 
// Changed the interface. The new interface will tell whether
// the address is correct or not. 
// Chen Ding, 06/18/2007
//
static inline int aos_addr_to_u32(
		const char *ipAddrDotNotation, 
		u32 *addr)
{
	//
	// If the input address is invalid, it returns negative;
	// otherwise, it returns the binary data in network byte order.
	//
	struct in_addr inp;

	aos_assert_r(ipAddrDotNotation, -1);
	aos_assert_r(addr, -1);
	aos_assert_r(inet_aton(ipAddrDotNotation, &inp), -1);
	
	*addr = inp.s_addr;
	return 0;
}


static inline const char *aos_addr_to_str(const u32 addr)
{
	// 
	// It returns the numbers-and-dots notation of the addr 'addr'.
	//
	struct in_addr inp;
	inp.s_addr = addr;
	return inet_ntoa(inp);
}

#endif

#endif

