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
#include "UtilTime/TFYYYYMM.h"

#include "UtilTime/TimeUtil.h"


AosTFYYYYMM::AosTFYYYYMM(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_YYYYMM, eYYYYMM, flag)
{
}


AosTFYYYYMM::~AosTFYYYYMM()
{
}


bool
AosTFYYYYMM::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data', and
	// its converted value is saved in 'value'.
	aos_assert_r(data && len >= 6, false);

	switch (target_format)
	{
	case eYYYYMMDDHHMMSS:
		 {
			 OmnString ss(data, 6);
			 ss << "01000000";
			 value.setStr(ss);
			 return true;
		 }

	case eYYYYMMDD:
		 {
			 OmnString ss(data, 6);
			 ss << "01";
			 value.setStr(ss);
		 	 return true;
		 }

	case eYYYYMM:
		 {
			 OmnString ss(data, 6);
			 value.setStr(ss);
		 	 return true;
		 }

	case eYYMMDDHHMMSS:
		 {
			 OmnString ss(&data[2], 4);
			 ss << "01000000";
			 value.setStr(ss);
		 	 return true;
		 }
	
	case eYYMMDD:
		 {
			 OmnString ss(&data[2], 4);
			 ss << "01";
			 value.setStr(ss);
		 	 return true;
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
AosTFYYYYMM::str2EpochTime(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// 		yyyymmdd
	// This function converts this string into a u32 Epoch Time representation
	// of the time.
	aos_assert_r(data && len >= 6, 0);
	
	OmnString yy = OmnString(data, 4);
	int year = yy.toInt();

	// Retrieve month
	OmnString mm = OmnString(&data[4], 2);
	int month = mm.toInt();
	if (month < 0 || month > 12) month = 0;
	
	// Retrieve day
	int day = 1;
	int hour = 0;
	int min = 0;
	int sec = 0;
	
	return AosTimeUtil::mktimeNew(year, month, day, hour, min, sec);
}


int
AosTFYYYYMM::str2EpochDay(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// 		yyyymmddhhmmss
	// This function converts this string into a u16 Epoch Day representation
	// of the time.
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_DAY;
}


int
AosTFYYYYMM::str2EpochHour(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
}


int
AosTFYYYYMM::str2EpochMonth(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return AosTimeUtil::getEpochMonth(second);
}
