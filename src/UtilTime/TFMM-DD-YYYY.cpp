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
#include "UtilTime/TFMM-DD-YYYY.h"

#include "UtilTime/TimeUtil.h"


AosTFMMDDYYYY::AosTFMMDDYYYY(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_MMDDYYYY, eMMDDYYYY, flag)
{
}


AosTFMMDDYYYY::~AosTFMMDDYYYY()
{
}


bool
AosTFMMDDYYYY::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data', and
	// its converted value is saved in 'value'.
	aos_assert_r(data && len >= 14, false);

	switch (target_format)
	{
	case eYYYYMMDDHHMMSS:	
		 {
			 OmnNotImplementedYet;
			 return false;
		 }

	case eYYYYMMDD:
		 {
			 OmnNotImplementedYet;
		 	 return false;
		 }

	case eYYMMDDHHMMSS:
		 {
			 OmnNotImplementedYet;
		 	 return false;
		 }

	case eYYMMDD:
		 {
			 OmnNotImplementedYet;
		 	 return false;
		 }

	case eEpochTime:
		 {
			 value.setI64(str2EpochTime(data, len));
		 	 return true;
		 }
			  
	case eEpochDay:
		 {
			 value.setI64(str2EpochDay(data, len));
			 return true;
		 }

	case eEpochHour:
		 {
			 value.setI64(str2EpochHour(data, len));
		 	 return true;
		 }
	
	case eEpochMonth:
		 {
			 value.setI64(str2EpochMonth(data, len));
		  	 return true;
		 }

	case eDayOfWeek:
		 {
		 	 i64 second = str2EpochTime(data, len);
			 value.setI64(AosTimeUtil::getDayOfWeek(second));
			 return true;
		 }

	case eDayOfMonth:
		 {
			 i64 second = str2EpochTime(data, len);
			 value.setI64(AosTimeUtil::getDayOfMonth(second));
		 	 return true;
		 }

	case eHourOfDay:
		 {
		 	 i64 second = str2EpochTime(data, len);
			 value.setI64(AosTimeUtil::getHourOfDay(second));
		 	 return true;
		 }

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_time_format") << enderr;
	return false;
}


i64
AosTFMMDDYYYY::str2EpochTime(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// mm-dd-yyyy 
	// This function converts this string into a u64 representation
	// of the time.
	aos_assert_r(data && len >= 10, 0);
	
	OmnString mm = OmnString(data, 2);
	int month = mm.toInt();
	if (month < 0 || month > 12) month = 0;

	OmnString dd = OmnString(&data[3], 2);
	int day = dd.toInt();
	if (day < 1 || day > 31) day = 1;

	OmnString yy = OmnString(&data[6], 4);
	int year = yy.toInt();

	int hour = 0; 
	int min = 0;
	int sec = 0; 
	
	return AosTimeUtil::mktimeNew(year, month, day, hour, min, sec);
}


int
AosTFMMDDYYYY::str2EpochDay(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_DAY;
}


int
AosTFMMDDYYYY::str2EpochHour(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
}


int
AosTFMMDDYYYY::str2EpochMonth(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return AosTimeUtil::getEpochMonth(second);
}

