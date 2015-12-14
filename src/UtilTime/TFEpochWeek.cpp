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
#include "UtilTime/TFEpochWeek.h"

#include "UtilTime/TimeUtil.h"

AosTFEpochWeek::AosTFEpochWeek(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_EPOCH_WEEK, eEpochWeek, flag)
{
}


AosTFEpochWeek::~AosTFEpochWeek()
{
}


bool
AosTFEpochWeek::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data' and
	// its converted value is saved in 'value'.
	// Ketty 2014/12/15
	aos_assert_r(data && len > 0, false);

	OmnString s(data, len);

	i64 epoch_week;
	u32 nn;
	s.parseInt64(0, nn, epoch_week);

	i64 epoch_day = epoch_week * 7;
	if(epoch_week>0)	epoch_day += 3;
	i64 second = epoch_day * AOSTIMEFORMAT_SECONDS_PRE_DAY;
	
	switch (target_format)
	{

	case eYYYY_MM_DDHHMMSS:
		 {
			 value.setStr(AosTimeUtil::epochToStrNew("%Y-%m-%d %H:%M:%S", second));
			 return true;
		 }
	
	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_time_format") << enderr;
	return false;
}



