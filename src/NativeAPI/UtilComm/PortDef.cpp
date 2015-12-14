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
//	This class is used to configure multiple ports. It is specified as:
//		nnn|$|nnn|$|
//	where nnn can be a single port or a range in the form of: 
//		mmm-mmm
//
// 	To simplify the programming, we limit that there can be at most
// 	eMaxPortRanges number of ranges specified. Additional ranges
// 	will be silently ignored.
//
// Modification History:
// 03/23/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/PortDef.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "util_c/strutil.h"


AosPortDef::AosPortDef()
:
mNumRanges(0),
mCrtIdx(0)
{
}


bool
AosPortDef::config(const OmnString &def, const int dft_low, const int dft_high)
{
	if (config(def)) return true;

	aos_assert_r(dft_low > 0, false);
	aos_assert_r(dft_high > 0, false);
	mLows[0] = dft_low;
	mHighs[0] = dft_high;
	mNumRanges = 1;
	return true;
}


bool
AosPortDef::config(const OmnString &def)
{
	char *parts[eMaxPortRanges];
	int num = aos_sstr_split((char *)def.data(), "|$|", parts, eMaxPortRanges);
	aos_assert_r(num >= 0, false);
	if (num == 0) return false;

	for (int i=0; i<num; i++)
	{
		aos_assert_r(aos_atoi_range(parts[i], '-', &mLows[i], &mHighs[i]) == 0, false);
	}
	mNumRanges = num;
	aos_str_split_releasemem(parts, num);
	return true;
}


void
AosPortDef::nextRange(int &low, int &high) 
{
	aos_assert(mNumRanges > 0);
	aos_assert(mCrtIdx < mNumRanges);
	low = mLows[mCrtIdx];
	high = mHighs[mCrtIdx];
	mCrtIdx++;
}


