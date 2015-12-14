////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IpAddr.cpp
// Description:
//	We have to prepare ourselves for IPv6. This class defines the type
//  for IP Addresses. Our intention is to make the rest of the system
//  unaware of whether it is using IPv4 or IPv6. This class currently 
//  is nothing but just a typedef. But in the future, it will implement
//  the IPv6 IP address (12 bytes long) and make it exactly the same
//  as if it were 4 bytes long.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/IpAddr.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/InetAddr.h"

#include "Util/ValList.h"


OmnIpAddr OmnIpAddr::eInvalidIpAddr(0, OmnIpAddr::eIPv4Int);
OmnIpAddr OmnIpAddr::eLoopbackAddr("127.0.0.1", OmnIpAddr::eIPv4DotNotation);


OmnIpAddr::OmnIpAddr(const OmnString &ipAddr, const AddrType t)
:
mIsGood(false)
{
	set(ipAddr, t);
}


void
OmnIpAddr::setLoopback()
{
	OmnInetAddr("127.0.0.1", mAddr);
	mIsGood = true;
}


bool
OmnIpAddr::set(const OmnString &ipAddr, const AddrType t)
{
	mIsGood = false;
	if (t == eIPv4Int)
	{
		mAddr = ::atoi(ipAddr.data());
		mIsGood = true;
		return true;
	}
	else if (t == eIPv4DotNotation)
	{
		mIsGood = OmnInetAddr(ipAddr.data(), mAddr);
		return mIsGood;
	}
	else 
	{
		OmnAlarm << "IP Address type not recognized: " << t << enderr;
		mAddr = 0;
		return false;
	}

	return true;
}


OmnString
OmnIpAddr::toString() const
{
	if(!mIsGood)
	{
		OmnString ret = "invalid addr:";
		ret << OmnInetDotAddr(mAddr);
		return ret;
	}
	return OmnInetDotAddr(mAddr);
}


bool
OmnIpAddr::isValidAddr(const OmnString &addr)
{
	const char *data = addr.data();
	int len = addr.length();
	int index = 0;
	int loop = 0;

	// Skip leading spaces
	while (index < len && (data[index] == ' ' || data[index] == '\t')) 
	{
		index++;
	}

	// Strim off tailing spaces
	// while (len > 0 && (data[len-1] == ' ' || data[len-1] == '\t')) len--;

	while (index < len && loop < 4)
	{
		// 
		// Expect all digits, maximum 3
		//
		int numDigits = 0;
		while (index < len && numDigits <= 3)
		{
			if (data[index] >= '0' && data[index] <= '9')
			{
				numDigits++;
				index++;
			}
			else
			{
				break;
			}
		}

		if (numDigits > 3)
		{
			return false;
		}

		// 
		// If it is the last segment (loop == 3), it is a good address
		// 
		if (loop == 3)
		{
			return true;
		}

		// 
		// The following character should be '.'
		//
		if (data[index] != '.')
		{
			return false;
		}

		loop++;
		index++;
	}

	return false;
}


bool
OmnIpAddr::isValidMask(const OmnString &mask)
{
	OmnIpAddr addr(mask);
	return isValidMask(addr);
}


bool
OmnIpAddr::isValidMask(const OmnIpAddr &mask)
{
	u32 value = mask.getIPv4();
	if (value == 0) return true;

	u32 m = (u32)0xffffffff;
	for (int i=0; i<32; i++)
	{
		if (htonl(m) == value)
		{
			return true;
		}

		m = (m << 1);
	}

	return false;
}


bool 
OmnIpAddr::isAddrIn(const OmnIpAddr &addr, 
					const OmnIpAddr &netaddr, 
					const OmnIpAddr &netmask)
{
	// 
	// It checks whether the addr 'addr' in the network 
	// defined by 'netaddr/netmask'. If yes, it returns true. 
	// Otherwise, it returns false. 
	// 
	u32 a = htonl(addr.getIPv4());
	u32 n = htonl(netaddr.getIPv4());
	u32 m = htonl(netmask.getIPv4());

	return (n & m) == (a & m);
}

			
