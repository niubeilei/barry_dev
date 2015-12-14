////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IPv6.h
// Description:
//	This class defines some inline functions that may be different 
//  for IPv4 or IPv6. It is intended to isolate the IPv4 and IPv6 
//  differences.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_IPv6_h
#define Omn_Porting_IPv6_h

#include "Debug/Debug.h"
#include "Debug/Error.h"


///////////////////////////////////////////////////////////////////////
//			IPv4
///////////////////////////////////////////////////////////////////////
#ifndef OMN_IPV6

#include "Alarm/Alarm.h"
#include "Util/IpAddr.h"





#ifdef OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#elif OMN_PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif





inline void	OmnSetIpAddr(struct sockaddr_in * const servAddr, 
						 const OmnIpAddr &ipAddr)
{
	//
	// Note that this function assumes that the value "getIPv4()" 
	// returns is already in network order. 
	//
//	(servAddr->sin_addr).s_addr = htons(ipAddr.getIPv4());
	(servAddr->sin_addr).s_addr = ipAddr.getIPv4();
}


inline OmnIpAddr OmnRetrieveIpAddr(const sockaddr_in &clientAddr)
{
	return OmnIpAddr(clientAddr.sin_addr.s_addr);
}


inline OmnIpAddr OmnGetIPAddrByName(const OmnString &hostname)
{
    struct hostent *hp = 0;
    unsigned int inetAddr;
    hp = gethostbyname(hostname.data());

    if (!hp)
    {
		OmnAlarm << "FailedToGetHostByName: " << hostname << enderr;
    	return OmnIpAddr::eInvalidIpAddr;
    }

	inetAddr = ((struct in_addr*)(hp->h_addr))->s_addr;
    return OmnIpAddr(inetAddr);
}















///////////////////////////////////////////////////////////////////////
//			IPv6
///////////////////////////////////////////////////////////////////////
#else
//
// Not implemented yet
//
#endif

#endif

