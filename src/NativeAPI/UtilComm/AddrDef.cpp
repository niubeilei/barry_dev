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
// 	AddrDef is used to define local IP addresses. Local addresses can be 
// 	specified as:
// 		all: all the local ip addresses
// 		loopback: 127.0.0.1
// 		interface:nnn|$|nnn|$|...
// 		addr:aaa|$|aaa|$|...
//
// 	To simplify the programming, we limit that there can be at most
// 	eMaxAddrs number of addresses specified. Additional addresses
// 	will be silently ignored.
//
// Modification History:
// 03/23/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/AddrDef.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "util_c/strutil.h"


AosAddrDef::AosAddrDef()
:
mNumAddrs(0),
mCrtIdx(0)
{
}


bool
AosAddrDef::config(const OmnString &def, const OmnString &dft)
{
	// Chen Ding, 05/04/2010
	if (config(def) || config(dft)) return true;

	OmnAlarm << "Invalid address specifications: " << def
		<< ". The default: " << dft << enderr;
	return false;
}


bool
AosAddrDef::config(const OmnString &def)
{
	if (def == "all")
	{
		OmnNotImplementedYet;
		return false;
	}

	if (def == "loopback")
	{
		mAddrs[0] = OmnIpAddr("127.0.0.1");
		mNumAddrs = 1;
		return true;
	}

	char *parts[2];
	int num = aos_str_split(def.data(), ':', parts, 2);
	if (num != 2)
	{
		aos_str_split_releasemem(parts, 2);

		// Assume 'def' is an IP address
		mAddrs[0] = OmnIpAddr(def.data());
		mNumAddrs = 1;
		return true;
	}

	OmnString type(parts[0]);
	if (type == "interface")
	{
		char *names[eMaxAddrs];
		num = aos_sstr_split(parts[1], "|$|", names, eMaxAddrs);
		aos_assert_r(num > 0, false);
		for (int i=0; i<num; i++)
		{
			OmnNotImplementedYet;
			aos_assert_r(mAddrs[i].isValid(), false);
		}
		mNumAddrs = num;
		aos_str_split_releasemem(parts, 2);
		aos_str_split_releasemem(names, num);
		return true;
	}

	if (type == "addr")
	{
		char *names[eMaxAddrs];
		num = aos_sstr_split(parts[1], "|$|", names, eMaxAddrs);
		aos_assert_r(num > 0, false);
		for (int i=0; i<num; i++)
		{
			mAddrs[i] = OmnIpAddr(names[i]);
			aos_assert_r(mAddrs[i].isValid(), false);
		}
		mNumAddrs = num;
		aos_str_split_releasemem(parts, 2);
		aos_str_split_releasemem(names, num);
		return true;
	}

	return false;
}


OmnIpAddr
AosAddrDef::nextAddr() 
{
	aos_assert_r(mNumAddrs > 0, OmnIpAddr("127.0.0.1"));
	aos_assert_r(mCrtIdx < mNumAddrs, OmnIpAddr("127.0.0.1"));
	return mAddrs[mCrtIdx++];
}

