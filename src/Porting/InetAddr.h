////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: InetAddr.h
// Description:
//	In order to use inet_addr(...), we need to include a file, which is
//  platform dependent. This file hides the dependency.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_InetAddr_h
#define Omn_Porting_InetAddr_h

//////////////////////  MICROSOFT  ////////////////////////////////////
#ifdef OMN_PLATFORM_MICROSOFT

#include <winsock2.h>

inline long OmnInetAddr(const char *ipAddrDotNotation)
{
	//
	// The function: inet_addr(...) from Microsoft converts the IPv4
	// dot notation into a long, in the network order. 
	//
	return inet_addr(ipAddrDotNotation);
}


inline OmnString OmnInetDotAddr(const int binAddr)
{
	// It returns the numbers-and-dots notation of the ip address binAddr.
	//
	in_addr inp;
	inp.S_un.S_addr = hton(binAddr);
	return inet_ntoa(inp);
}


//////////////////////  Linux       ////////////////////////////////////
#elif __KERNEL__
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
inline long OmnInetAddr(const char *ipAddrDotNotation)
{
	//
	// If the input address is invalid, it returns zero;
	// otherwise, it returns the binary  data in network byte order.
	//
	return in_aton(ipAddrDotNotation);
}


#elif OMN_PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Alarm/Alarm.h"

// 
// Changed the interface. The new interface will tell whether
// the address is correct or not. 
// Chen Ding, 06/18/2007
//
// inline long OmnInetAddr(const char *ipAddrDotNotation)
inline bool OmnInetAddr(const char *ipAddrDotNotation, u32 &addr)
{
	//
	// If the input address is invalid, it returns zero;
	// otherwise, it returns the binary  data in network byte order.
	//
	struct in_addr inp;

	if (!ipAddrDotNotation)
	{
		OmnAlarm << "Null pointer passed in" << enderr;
		return false;
	}

	if ( inet_aton(ipAddrDotNotation, &inp) == 0 )
	{
		return false;
	}
	
	addr = inp.s_addr;
	return true;
}


inline OmnString OmnInetDotAddr(const int binAddr)
{
	// It returns the numbers-and-dots notation of the ip address binAddr.
	//
	struct in_addr inp;
	inp.s_addr = binAddr;
	return inet_ntoa(inp);
}

#endif

#endif

