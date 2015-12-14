////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MacAddr.cpp
// Descrmaction:
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/MacAddr.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/InetAddr.h"
#include "Util/ValList.h"
#include "Util/StrParser.h"


OmnMacAddr OmnMacAddr::eInvalidMacAddr("00:00:00:00:00:00");
OmnMacAddr OmnMacAddr::eZeroMacAddr("00:00:00:00:00:00");


OmnMacAddr::OmnMacAddr(const char *data)
{
	if (!set(data))
	{
		memset(mAddr, 0, sizeof(mAddr));
	}
}


OmnMacAddr::OmnMacAddr(const OmnString &macAddr)
{
	if (!set(macAddr))
	{
		memset(mAddr, 0, sizeof(mAddr));
	}
}


bool
OmnMacAddr::set(const OmnString &macAddr)
{
	if (macAddr.length() == 6)
	{
		memcpy(mAddr, macAddr.data(), 6);
		return true;
	}

	OmnStrParser1 parser(macAddr, ":", false, false);
	OmnString w;
	for (int i=0; i<6; i++)
	{
		w = parser.nextWord("");
		if (w == "") return false;
		if (w.length() > 2) return false;

		const char *data = w.data();
		char c1 = getHexCharValue(data[0]);
		if (c1 < 0) return false;

		if (w.length() == 2)
		{
			char c2 = getHexCharValue(data[1]);
			if (c2 < 0) return false;
			mAddr[i] = (char)(((unsigned char)c1) << 4) + (unsigned char)c2;
		}
		else
		{
			mAddr[i] = c1;
		}
	}
		
	return true;
}


OmnString
OmnMacAddr::toString() const
{
	OmnString strMac;
	strMac.setLength(17);
	sprintf(strMac.getBuffer(),
			"%02x:%02x:%02x:%02x:%02x:%02x",
			mAddr[0],
			mAddr[1],
			mAddr[2],
			mAddr[3],
			mAddr[4],
			mAddr[5]);	
	
	return strMac;
}


bool
OmnMacAddr::isValidAddr(const OmnString &addr)
{
	OmnMacAddr a(addr);
	return a.isValid();
}


bool 
OmnMacAddr::isSingleCast() const 
{
	return !mAddr[0]%2;
}

bool 
OmnMacAddr::isGroupCast() const 
{
	return mAddr[0]%2;
}

bool 
OmnMacAddr::isLocal() const 
{
	return !mAddr[0]/2%2;
}


bool
OmnMacAddr::isValid() const
{
	return mAddr[0] != 0 ||
		   mAddr[1] != 0 ||
		   mAddr[2] != 0 ||
		   mAddr[3] != 0 ||
		   mAddr[4] != 0 ||
		   mAddr[5] != 0;
}

