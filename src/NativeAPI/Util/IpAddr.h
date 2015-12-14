////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IpAddr.h
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

#ifndef Omn_Util_IpAddr_h
#define Omn_Util_IpAddr_h

#include "Util/Itoa.h"
#include "Util/String.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef OmnInvalidIpAddr
#define OmnInvalidIpAddr OmnIpAddr::eInvalidIpAddr
#endif

//
// Currently it supports IPv4 only
//
class OmnIpAddr 
{
public:
	static OmnIpAddr		eInvalidIpAddr;
	static OmnIpAddr		eLoopbackAddr;

	enum AddrType
	{
		eIPv4Int,				// IPv4 expressed as integer char string
		eIPv4DotNotation,		// e.g., 10.10.10.1
		eIPv6DotNotation		// e.g., 10.10.10.10.10.10.10.10.10.10.10.10
	};

private:
	u32			mAddr;		// The address in network order
	bool		mIsGood;

public:
	OmnIpAddr()
		:
	mAddr(0),
	mIsGood(false)
	{
	}

	OmnIpAddr(const unsigned long addr, const AddrType t=eIPv4Int)
		:
	mAddr(addr),
	mIsGood(true)
	{}

	OmnIpAddr(const OmnString &data, const AddrType t = eIPv4DotNotation);

	~OmnIpAddr() {}

	bool			set(const OmnString &str, 
						const AddrType t = eIPv4DotNotation);
	bool			set(const unsigned long &v) {mAddr = v; mIsGood = true;return true;}
	unsigned int	getIPv4() const {return mAddr;}
	OmnString		toString() const;
	unsigned int	toInt() const {return mAddr;}
	void			setLoopback();
	u32 *			getAddr() {return &mAddr;}

	uint getHashKey() const {return (mAddr & 0xff) + ((mAddr >> 16) & 0xff);}
	bool operator == (const OmnIpAddr &rhs) const
	{
		if(!rhs.mIsGood && !mIsGood)
		{
			return true;
		} 
		return mAddr == rhs.mAddr && (rhs.mIsGood == mIsGood);
	}

	bool operator != (const OmnIpAddr &rhs) const
	{
		if(!rhs.mIsGood && !mIsGood)
		{
			return false;
		} 
		return !(mAddr == rhs.mAddr && (rhs.mIsGood == mIsGood));
	}

	bool isValid() const {return mIsGood;}

	bool operator !() const {return mAddr == 0;}
	bool operator ()() const {return mAddr != 0;}
	operator int() const {return (int)mAddr;}
	operator const char *() const {return toString().data();}

	bool isLoopback() const {return mIsGood && *this == eLoopbackAddr;}

	static bool isValidAddr(const OmnString &addr);
	static bool isValidMask(const OmnIpAddr &mask);
	static bool isValidMask(const OmnString &mask);
	static bool isAddrIn(const OmnIpAddr &addr, 
						 const OmnIpAddr &netaddr, 
						 const OmnIpAddr &netmask);
};
#endif
