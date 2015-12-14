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
#include "UtilTime/TFMM_DD_YYYY_HH_MM.h"

#include "UtilTime/TimeUtil.h"


AosTFMM_DD_YYYY_HH_MM::AosTFMM_DD_YYYY_HH_MM(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_MM_DD_YYYY_HH_MM, eMM_DD_YYYY_HH_MM, flag)
{
}


AosTFMM_DD_YYYY_HH_MM::~AosTFMM_DD_YYYY_HH_MM()
{
}


bool
AosTFMM_DD_YYYY_HH_MM::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data', and
	// its converted value is saved in 'value'.
	aos_assert_r(data && len >= 19, false);

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
AosTFMM_DD_YYYY_HH_MM::str2EpochTime(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// mm/dd/yyyy hh:mm
	// This function converts this string into a u64 representation
	// of the time.
	aos_assert_r(data && len >= 16, 0);

	int year, month, day, hour, min, sec;
	sec = 0;
	sscanf(data, "%d/%d/%d %d:%d", &month, &day, &year, &hour, &min);
	return AosTimeUtil::mktimeNew(year, month, day, hour, min, sec);
}


int
AosTFMM_DD_YYYY_HH_MM::str2EpochDay(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_DAY;
}


int
AosTFMM_DD_YYYY_HH_MM::str2EpochHour(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
}


int
AosTFMM_DD_YYYY_HH_MM::str2EpochMonth(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return AosTimeUtil::getEpochMonth(second);
}

