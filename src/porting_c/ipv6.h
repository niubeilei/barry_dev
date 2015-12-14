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
//	This class defines some inline functions that may be different 
//  for IPv4 or IPv6. It is intended to isolate the IPv4 and IPv6 
//  differences.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_porting_ipv6_h
#define Omn_porting_ipv6_h

#include "alarm_c/alarm.h"
#include "util_c/types.h"


///////////////////////////////////////////////////////////////////////
//			IPv6
///////////////////////////////////////////////////////////////////////
#ifdef OMN_IPV6
// Not implemented yet










#else
///////////////////////////////////////////////////////////////////////
//			IPv6
///////////////////////////////////////////////////////////////////////





#ifdef OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#elif OMN_PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif





static inline void	aos_set_addr(
		struct sockaddr_in * const servAddr, 
		const u32 addr)
{
	//
	// Note that this function assumes that the value "getIPv4()" 
	// returns is already in network order. 
	//
	(servAddr->sin_addr).s_addr = addr;
}


static inline u32 aos_retrieve_addr(const struct sockaddr_in *clientAddr)
{
	return clientAddr->sin_addr.s_addr;
}


static inline u32 aos_get_addr_by_name(const char * const hostname)
{
    struct hostent *hp = gethostbyname((const char *)hostname);
	aos_assert_r(hp, 0);
	return ((struct in_addr*)(hp->h_addr))->s_addr;
}



#endif	// of IPv6

#endif	// of the file

