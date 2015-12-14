////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MacAddr.h
// Description:
//	This class defines the type for Mac Addresses.
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_MacAddr_h
#define Omn_Util_MacAddr_h

#include "Util/Itoa.h"
#include "Util/String.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef OmnInvalidMacAddr
#define OmnInvalidMacAddr OmnMacAddr::eInvalidMacAddr
#endif

//
// Currently it supports MACv4 only
//
class OmnMacAddr 
{
public:
	static OmnMacAddr		eInvalidMacAddr;
	static OmnMacAddr		eZeroMacAddr;

private:
	unsigned char mAddr[6];		// The address in network order

public:
	OmnMacAddr()
	{
		memset(mAddr,0,sizeof(unsigned char)*6);
	}

	OmnMacAddr(const OmnString &data);
	OmnMacAddr(const char *data);

	~OmnMacAddr() {}

	bool			set(const OmnString &str);

	void			getMAC(unsigned char macValue[6]) const {memcpy(macValue,mAddr,6);}
	OmnString		toString() const;
	void			setLoopback();

	bool operator == (const OmnMacAddr &rhs) const
	{
		return !memcmp(mAddr,rhs.mAddr,6);
	}

	bool operator != (const OmnMacAddr &rhs) const
	{
		return memcmp(mAddr,rhs.mAddr,6);
	}

	bool isValid() const;

	bool operator !() const {return *this != eZeroMacAddr;}
	bool operator ()() const {return *this == eZeroMacAddr;}

//	bool isLoopback() const {return *this == eLoopbackAddr;}

	static bool isValidAddr(const OmnString &addr);

	bool isSingleCast() const;
	bool isGroupCast() const; 
	bool isLocal() const ;

private:
	inline static bool isHexChar(const char c)
	{
		return (c >= '0' && c <= '9') || 
			   (c >= 'a' && c <= 'f') || 
			   (c >= 'A' && c <= 'F');
	}

	inline static char getHexCharValue(const char c)
	{
		if(c >= '0' && c <= '9') 
		{
			return c - '0';
		}
		
		if(c >= 'A' && c <= 'F')
		{
			return c - 'A' + 10;
		}

		if(c >= 'a' && c <= 'f')
		{
			return c - 'a' + 10;
		}
		
		return -1;
	}
	
};
#endif
