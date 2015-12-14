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
#include "UtilTime/TFEpochMonth.h"

#include "UtilTime/TimeUtil.h"

AosTFEpochMonth::AosTFEpochMonth(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_EPOCH_MONTH, eEpochMonth, flag)
{
}


AosTFEpochMonth::~AosTFEpochMonth()
{
}


bool
AosTFEpochMonth::convert(
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
	i64 epoch_month;
	u32 nn;
	s.parseInt64(0, nn, epoch_month);

	int year = epoch_month/ 12 + 1970;
	int real_month = epoch_month % 12 + 1;
	OmnString time_str, month_str;

	if (real_month > 9)
		month_str << real_month;
	else
		month_str << "0" << real_month;

	switch (target_format)
	{

	case eYYYY_MM_DDHHMMSS:
		 {
			 //value.setValue(AosTimeUtil::epochToStrNew("%Y-%m-%d %H:%M:%S", second));
		 	 time_str << year << "-" << month_str << "-01 00:00:00";
			 value.setStr(time_str);
			 return true;
		 }
	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_time_format") << enderr;
	return false;
}


