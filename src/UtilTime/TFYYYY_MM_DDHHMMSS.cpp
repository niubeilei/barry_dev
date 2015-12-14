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
#include "UtilTime/TFYYYY_MM_DDHHMMSS.h"

#include "UtilTime/TimeUtil.h"


AosTFYYYY_MM_DDHHMMSS::AosTFYYYY_MM_DDHHMMSS(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_YYYY_MM_DDHHMMSS, eYYYY_MM_DDHHMMSS, flag)
{
}


AosTFYYYY_MM_DDHHMMSS::~AosTFYYYY_MM_DDHHMMSS()
{
}


bool
AosTFYYYY_MM_DDHHMMSS::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data', and
	// its converted value is saved in 'value'.
	//aos_assert_r(data && len >= 19, false);
	if (!(data && len >= 8))
	{
		AosSetEntityError(rdata, "TFYYYY_MM_DDHHMMSS_data_length_error", 
				"UtilTime", "UtilTime_TFYYYY_MM_DDHHMMSS") << "length: " << len << enderr;
		return false;
	}

	switch (target_format)
	{
	case eYYYYMMDDHHMMSS:	
		 {
			 int year, month, day, hour, min, sec;
			 year = month = day = hour = min = sec = 0;
			 sscanf(data, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
			
			 OmnString s(14, 0, true);
			 sprintf(s.getBuffer(), "%04d%02d%02d%02d%02d%02d", year, month, day, hour, min, sec);
			 value.setStr(s);
			 return true;
		 }

	case eYYYYMMDD:
		 {
			 int year, month, day, hour, min, sec;
			 year = month = day = hour = min = sec = 0;
			 sscanf(data, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
			
			 OmnString s(8, 0, true);
			 sprintf(s.getBuffer(), "%04d%02d%02d", year, month, day);
			 value.setStr(s);
			 return true;
		 }

	case eYYMMDDHHMMSS:
		 {
			 int year, month, day, hour, min, sec;
			 year = month = day = hour = min = sec = 0;
			 sscanf(data, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
			
			 OmnString s(12, 0, true);
			 sprintf(s.getBuffer(), "%02d%02d%02d%02d%02d%02d", year%100, month, day, hour, min, sec);
			 value.setStr(s);
			 return true;
		 }

	case eYYMMDD:
		 {
			 int year, month, day, hour, min, sec;
			 year = month = day = hour = min = sec = 0;
			 sscanf(data, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
					
			 OmnString s(6, 0, true);
			 sprintf(s.getBuffer(), "%02d%02d%02d", year%100, month, day);
			 value.setStr(s);
			 return true;
		 }
	
	case eYYYY_MM_DDHHMMSS:
		 {
			 int year, month, day, hour, min, sec;
			 year = month = day = hour = min = sec = 0;
			 sscanf(data, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
					
			 OmnString s(19, 0, true);
			 sprintf(s.getBuffer(), "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);
			 value.setStr(s);
			 return true;
		 }

	case eEpochTime:
		 {
			 value.setI64(str2EpochTime(data, len));
		 	 return true;
		 }
			  
	case eEpochDay:
		 {
			 value.setI64((u64)(str2EpochDay(data, len)));
			 return true;
		 }

	case eEpochHour:
		 {
			 value.setI64((u64)(str2EpochHour(data, len)));
		 	 return true;
		 }
	
	case eEpochWeek:
		 {
		 	// Ketty 2014/12/15
		 	i64 second = str2EpochTime(data, len);
			value.setI64(AosTimeUtil::getEpochWeek(second));
		 	return true;
		 }
	
	case eEpochMonth:
		 {
			 value.setI64((u64)(str2EpochMonth(data, len)));
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
AosTFYYYY_MM_DDHHMMSS::str2EpochTime(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// yyyy-mm-dd hh:mm:ss 
	// This function converts this string into a u64 representation
	// of the time.
	//aos_assert_r(data && len >= 19, 0);


	/*
	OmnString yy = OmnString(data, 4);
	int year = yy.toInt();
	
	OmnString mon = OmnString(&data[5], 2);
	int month = mon.toInt();
	if (month < 0 || month > 12) month = 0;

	OmnString dd = OmnString(&data[8], 2);
	int day = dd.toInt();
	if (day < 1 || day > 31) day = 1;

	OmnString hh = OmnString(&data[11], 2);
	int hour = hh.toInt();
	if (hour < 0 || hour > 24) hour = 0; 

	OmnString mm = OmnString(&data[14], 2);
	int min = mm.toInt();
	if (min < 0 || min > 59) min = 0;

	OmnString ss = OmnString(&data[17], 2);
	int sec = ss.toInt();
	if (sec < 0 || sec > 59) sec = 0; 
	*/

//OmnScreen << " ======================= time  is " << year << "-" << month << "-" << day << " " << hour << ":" << min << ":" << sec << endl;
	
	aos_assert_r(data && len >= 8, 0);
	int year, month, day, hour, min, sec;
	year = month = day = hour = min = sec = 0;
	sscanf(data, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
	return AosTimeUtil::mktimeNew(year, month, day, hour, min, sec);
}


int
AosTFYYYY_MM_DDHHMMSS::str2EpochDay(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_DAY;
}

int
AosTFYYYY_MM_DDHHMMSS::str2EpochWeek(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_WEEK;
	/*struct tm tm_time;
	strptime(data, "%Y-%m-%d %H:%M:%S", &tm_time);
	char dest[100] = {0};
	strftime(dest, sizeof(dest) - 1, "U%", &tm_time);
	return atoi(dest);*/
}

int
AosTFYYYY_MM_DDHHMMSS::str2EpochHour(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
}


int
AosTFYYYY_MM_DDHHMMSS::str2EpochMonth(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return AosTimeUtil::getEpochMonth(second);
}

