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
// 08/12/2012 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TFEpochYear.h"

#include "UtilTime/TimeUtil.h"

AosTFEpochYear::AosTFEpochYear(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_EPOCH_YEAR, eEpochYear, flag)
{
}


AosTFEpochYear::~AosTFEpochYear()
{
}


bool
AosTFEpochYear::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data' and
	// its converted value is saved in 'value'.
	// Ketty 2014/11/04
	aos_assert_r(data && len > 0, false);

	OmnString s(data, len);

	// Chen Ding, 2014/01/29
	i64 epoch_year;
	u32 nn;
	s.parseInt64(0, nn, epoch_year);

	i64 real_year = 1970 + epoch_year;
	OmnString time_str;

	switch (target_format)
	{

	case eYYYY_MM_DDHHMMSS:
		 {
			 //value.setValue(AosTimeUtil::epochToStrNew("%Y-%m-%d %H:%M:%S", second));
		 	 time_str << real_year << "-01-01 00:00:00";
			 value.setStr(time_str);
			 return true;
		 }
	
	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_time_format") << enderr;
	return false;
}

