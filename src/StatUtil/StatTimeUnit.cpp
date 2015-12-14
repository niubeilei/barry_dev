////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatTimeUnit.h"
#include "Rundata/Rundata.h"


static u64 st_time1 = 0;
static u64 st_time2 = 0;
static u64 st_time3 = 0;
static u64 st_time4 = 0;
static u64 st_time5 = 0;
static u64 st_time6 = 0;
static u64 st_time7 = 0;
	
AosStatTimeUnit::ParseFun  AosStatTimeUnit::smParseFuncs[AosStatTimeUnit::eInvalid][AosStatTimeUnit::eInvalid];
bool AosStatTimeUnit::smInited = false;

bool
AosStatTimeUnit::isTimeUnitStr(const OmnString &str)
{

	if(str == AOS_TIMEUNIT_EPOCHTIME)	return true;
	if(str == AOS_TIMEUNIT_HOUR)		return true;
	if(str == AOS_TIMEUNIT_DAY)			return true;
	if(str == AOS_TIMEUNIT_WEEK)		return true;
	if(str == AOS_TIMEUNIT_MONTH)		return true;
	if(str == AOS_TIMEUNIT_YEAR)		return true;
	
	if(str == "date_hour")	return true;
	//if(str == "Hour")	return true;
	if(str == "date_day")	return true;
	//if(str == "Day")	return true;
	if(str == "date_week")	return true;
	//if(str == "Week")	return true;
	if(str == "date_month")	return true;
	//if(str == "Month")	return true;
	if(str == "year")	return true;
	if(str == "Year")	return true;

	return false;
}


AosStatTimeUnit::E
AosStatTimeUnit::getTimeUnit(const OmnString &field)
{
	//arvin 2015.11.06
	//Jimodb-1138
	if(field == "")							return eAllTime;	
	if(field == AOS_TIMEUNIT_EPOCHTIME)		return eEpochTime;
	if(field == AOS_TIMEUNIT_HOUR)			return eEpochHour;
	if(field == AOS_TIMEUNIT_DAY)			return eEpochDay;
	if(field == AOS_TIMEUNIT_WEEK)			return eEpochWeek;
	if(field == AOS_TIMEUNIT_MONTH)			return eEpochMonth;
	if(field == AOS_TIMEUNIT_YEAR)			return eEpochYear;
	if(field == AOS_TIMEUNIT_ALLTIME)		return eAllTime;
	
	if(field == "date_hour")	return eEpochHour;
	//if(field == "Hour")	return eEpochHour;
	if(field == "date_day")	return eEpochDay;
	//if(field == "Day")	return eEpochDay;
	if(field == "date_week")	return eEpochWeek;
	//if(field == "Week")	return eEpochWeek;
	if(field == "date_month")	return eEpochMonth;
	//if(field == "Month")	return eEpochMonth;
	if(field == "year")	return eEpochYear;
	if(field == "Year")	return eEpochYear;

	OmnShouldNeverComeHere;
	return eInvalid;
}


OmnString
AosStatTimeUnit::toStr(const E cond)
{
	
	switch(cond)
	{
	case eEpochTime:
		return AOS_TIMEUNIT_EPOCHTIME;
	
	case eEpochHour:
		return AOS_TIMEUNIT_HOUR;

	case eEpochDay:
		return AOS_TIMEUNIT_DAY;
	
	case eEpochWeek:
		return AOS_TIMEUNIT_WEEK;
	
	case eEpochMonth:
		return AOS_TIMEUNIT_MONTH;
	
	case eEpochYear:
		return AOS_TIMEUNIT_YEAR;
	
	case eAllTime:
		return AOS_TIMEUNIT_ALLTIME;

	default:
		break;	
	}
	
	OmnShouldNeverComeHere;
	return "";	
}

void
AosStatTimeUnit::init()
{
	if(smInited)	return;
	smInited = true;
	
	smParseFuncs[eEpochTime][eEpochTime] = epochTimeToEpochTime;

	smParseFuncs[eEpochTime][eEpochHour] = epochTimeToEpochHour;
	smParseFuncs[eEpochHour][eEpochHour] = epochHourToEpochHour;

	smParseFuncs[eEpochTime][eEpochDay] = epochTimeToEpochDay;
	smParseFuncs[eEpochHour][eEpochDay] = epochHourToEpochDay;
	smParseFuncs[eEpochDay][eEpochDay] = epochDayToEpochDay;

	smParseFuncs[eEpochTime][eEpochWeek] = epochTimeToEpochWeek;
	smParseFuncs[eEpochHour][eEpochWeek] = epochHourToEpochWeek;
	smParseFuncs[eEpochDay][eEpochWeek] = epochDayToEpochWeek;
	smParseFuncs[eEpochWeek][eEpochWeek] = epochWeekToEpochWeek;
	
	smParseFuncs[eEpochTime][eEpochMonth] = epochTimeToEpochMonth;
	smParseFuncs[eEpochHour][eEpochMonth] = epochHourToEpochMonth;
	smParseFuncs[eEpochDay][eEpochMonth] = epochDayToEpochMonth;
	smParseFuncs[eEpochWeek][eEpochMonth] = epochWeekToEpochMonth;
	//arvin 2015.08.10
	//JIMODB-334
	smParseFuncs[eEpochMonth][eEpochMonth] = epochMonthToEpochMonth;
	smParseFuncs[eEpochYear][eEpochYear] = epochYearToEpochYear;
	
	smParseFuncs[eEpochTime][eEpochYear] = epochTimeToEpochYear;
	smParseFuncs[eEpochHour][eEpochYear] = epochHourToEpochYear;
	smParseFuncs[eEpochDay][eEpochYear] = epochDayToEpochYear;
	smParseFuncs[eEpochWeek][eEpochYear] = epochWeekToEpochYear;
	
	smParseFuncs[eEpochTime][eAllTime] = parseToAllTime;
	smParseFuncs[eEpochDay][eAllTime] = parseToAllTime;
	smParseFuncs[eEpochHour][eAllTime] = parseToAllTime;
	smParseFuncs[eEpochWeek][eAllTime] = parseToAllTime;
	smParseFuncs[eEpochMonth][eAllTime] = parseToAllTime;
	smParseFuncs[eEpochYear][eAllTime] = parseToAllTime;
	smParseFuncs[eAllTime][eAllTime] = parseToAllTime;
	
}

int
AosStatTimeUnit::parseTimeValue(
		const int crt_epoch_time,
		const E from_time_unit,
		const E to_time_unit)
{
	aos_assert_r(smParseFuncs[from_time_unit][to_time_unit], -1);
	return smParseFuncs[from_time_unit][to_time_unit](crt_epoch_time);
}
	

bool
AosStatTimeUnit::isCompatible(const E left_cond, const E right_cond)
{
	switch(left_cond)
	{
	case eEpochTime: 
		return true;
	
	case eEpochHour:
		{
			switch(right_cond)
			{
			case eEpochTime:
				return false;
			
			case eEpochHour:
			case eEpochDay:
			case eEpochWeek:
			case eEpochMonth:
			case eEpochYear:
			case eAllTime:
				return true;
		
			default:
				break;
				
			}
			
			OmnShouldNeverComeHere;
			return false;
		}
	case eEpochDay:
		{
			switch(right_cond)
			{
			case eEpochTime:
			case eEpochHour:
				return false;
			
			case eEpochDay:
			case eEpochWeek:
			case eEpochMonth:
			case eEpochYear:
			case eAllTime:
				return true;
		
			default:
				break;
			}
			
			OmnShouldNeverComeHere;
			return false;
		}

	case eEpochWeek:
		{
			switch(right_cond)
			{
			case eEpochTime:
			case eEpochHour:
			case eEpochDay:
				return false;
			
			case eEpochWeek:
			case eEpochMonth:
			case eEpochYear:
			case eAllTime:
				return true;
			
			default:
				break;
			}
			
			OmnShouldNeverComeHere;
			return false;
		}
	
	case eEpochMonth:
		{
			switch(right_cond)
			{
			case eEpochTime:
			case eEpochHour:
			case eEpochDay:
			case eEpochWeek:
				return false;

			case eEpochMonth:
			case eEpochYear:
			case eAllTime:
				return true;
			
			default:
				break;
			}
			
			OmnShouldNeverComeHere;
			return false;
		}

	case eEpochYear:
		{
			switch(right_cond)
			{
			case eEpochTime:
			case eEpochHour:
			case eEpochDay:
			case eEpochWeek:
			case eEpochMonth:
				return false;
			
			case eEpochYear:
			case eAllTime:
				return true;
			
			default:
				break;
			}
			
			OmnShouldNeverComeHere;
			return false;
		}
	
	case eAllTime:
		{
			switch(right_cond)
			{
			case eEpochTime:
			case eEpochHour:
			case eEpochDay:
			case eEpochWeek:
			case eEpochMonth:
			case eEpochYear:
				return false;
			
			case eAllTime:
				return true;

			default:
				break;
			}
			
			OmnShouldNeverComeHere;
			return false;
		}
	
	default:
		break;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


int
AosStatTimeUnit::epochTimeToEpochTime(const int crt_epoch_time)
{
	return crt_epoch_time;
}

	
int
AosStatTimeUnit::epochTimeToEpochHour(const int crt_epoch_time)
{
	return crt_epoch_time / eSecondsPerHour; 
}

int
AosStatTimeUnit::epochHourToEpochHour(const int crt_epoch_time)
{
	return crt_epoch_time;
}

int
AosStatTimeUnit::epochTimeToEpochDay(const int crt_epoch_time)
{
	return crt_epoch_time / AosTimeUtil::eSecondsPerDay; 
}


int
AosStatTimeUnit::epochHourToEpochDay(const int crt_epoch_time)
{
	//return crt_epoch_time / eHoursPerDay;
	return crt_epoch_time / 24;
}


int
AosStatTimeUnit::epochDayToEpochDay(const int crt_epoch_time)
{
	return crt_epoch_time;
}


int
AosStatTimeUnit::epochTimeToEpochWeek(const int crt_epoch_time)
{
	return AosTimeUtil::getEpochWeek(crt_epoch_time);
}

int
AosStatTimeUnit::epochHourToEpochWeek(const int crt_epoch_time)
{
	int epoch_time = crt_epoch_time * eSecondsPerHour; 
	return AosTimeUtil::getEpochWeek(epoch_time);
}


int
AosStatTimeUnit::epochDayToEpochWeek(const int crt_epoch_time)
{
	int epoch_time = crt_epoch_time * AosTimeUtil::eSecondsPerDay; 
	return AosTimeUtil::getEpochWeek(epoch_time);
}

int
AosStatTimeUnit::epochWeekToEpochWeek(const int crt_epoch_time)
{
	return crt_epoch_time;
}


int
AosStatTimeUnit::epochTimeToEpochMonth(const int crt_epoch_time)
{
	return AosTimeUtil::getEpochMonth(crt_epoch_time);
}

int
AosStatTimeUnit::epochHourToEpochMonth(const int crt_epoch_time)
{
	int epoch_time = crt_epoch_time * eSecondsPerHour; 
	return AosTimeUtil::getEpochMonth(epoch_time);
}

int
AosStatTimeUnit::epochDayToEpochMonth(const int crt_epoch_time)
{
	int epoch_time = crt_epoch_time * AosTimeUtil::eSecondsPerDay; 
	return AosTimeUtil::getEpochMonth(epoch_time);
}

int
AosStatTimeUnit::epochWeekToEpochMonth(const int crt_epoch_time)
{
	// crt_epoch_time is epoch_week. 
	// small than 1970-01-03. epoch_week is 0.
	// 1970/01/01 is Thursday.
	int epoch_day = crt_epoch_time * 7;
	if(crt_epoch_time >0)	epoch_day += 3;
	
	int epoch_time = epoch_day * AosTimeUtil::eSecondsPerDay; 
	return AosTimeUtil::getEpochMonth(epoch_time);
}
//arvin 2015.08.10
//JIMODB-334
int
AosStatTimeUnit::epochMonthToEpochMonth(const int crt_epoch_time)
{
	return crt_epoch_time;
}

int
AosStatTimeUnit::epochYearToEpochYear(const int crt_epoch_time)
{
	return crt_epoch_time;
}

int
AosStatTimeUnit::epochTimeToEpochYear(const int crt_epoch_time)
{
	return AosTimeUtil::getEpochYear(crt_epoch_time);
}

int
AosStatTimeUnit::epochHourToEpochYear(const int crt_epoch_time)
{
	int epoch_time = crt_epoch_time * eSecondsPerHour; 
	return AosTimeUtil::getEpochYear(epoch_time);
}

int
AosStatTimeUnit::epochDayToEpochYear(const int crt_epoch_time)
{
	int epoch_time = crt_epoch_time * AosTimeUtil::eSecondsPerDay; 
	return AosTimeUtil::getEpochYear(epoch_time);
}

int
AosStatTimeUnit::epochWeekToEpochYear(const int crt_epoch_time)
{
	// crt_epoch_time is epoch_week. 
	// small than 1970-01-03. epoch_week is 0.
	// 1970/01/01 is Thursday.
	int epoch_day = crt_epoch_time * 7;
	if(crt_epoch_time >0)	epoch_day += 3;
	
	int epoch_time = epoch_day * AosTimeUtil::eSecondsPerDay; 
	return AosTimeUtil::getEpochYear(epoch_time);
}


int
AosStatTimeUnit::parseToAllTime(const int crt_epoch_time)
{
	return 0;
}


void
AosStatTimeUnit::KettyInitPrintTime()
{
	st_time1 = 0;
	st_time2 = 0;
	st_time3 = 0;
	st_time4 = 0;
	st_time5 = 0;
	st_time6 = 0;
	st_time7 = 0;
}

void
AosStatTimeUnit::KettyPrintTime()
{
	cout << "Times: "
		<< "st_time1 = " << st_time1 << "; " 
		<< "st_time2 = " << st_time2 << "; " 
		<< "st_time3 = " << st_time3 << "; " 
		<< "st_time4 = " << st_time4 << "; " 
		<< "st_time5 = " << st_time5 << "; " 
		<< "st_time6 = " << st_time6 << "; " 
		<< "st_time7 = " << st_time7 << "; " 
		<< endl;
}

OmnString
AosStatTimeUnit::convertToCalendarTime(const int64_t epoch_time, 
									AosStatTimeUnit::E timeUnit)
{
	bool rslt;
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosValueRslt value;
	
	AosTimeFormat::E fromFormat;
	AosTimeFormat::E targetFormat = AosTimeFormat::eYYYY_MM_DDHHMMSS;
	switch (timeUnit)
	{
		case eEpochTime:
			fromFormat = AosTimeFormat::eEpochTime;
			break;

		case eEpochHour:
			fromFormat = AosTimeFormat::eEpochHour;
			break;

		case eEpochDay:
			fromFormat = AosTimeFormat::eEpochDay;
			break;

		case eEpochWeek:
			fromFormat = AosTimeFormat::eEpochWeek;
			break;

		case eEpochMonth:
			fromFormat = AosTimeFormat::eEpochMonth;
			break;

		case eEpochYear:
			fromFormat = AosTimeFormat::eEpochYear;
			break;
		
		default:
			return "";
	}
	
	OmnString timeStr;
	timeStr << epoch_time;
	rslt = AosTimeFormat::convert(timeStr.data(), timeStr.length(),
			fromFormat, targetFormat, value, rdata);
	aos_assert_r(rslt, "");

	timeStr = value.getStr();
	return timeStr;
}

//////////////////////////////////////////////////////////////////////////////
//	JIMODB-989
//	arvin 2015.10.22
//	this function is match with transformTime (function in DataProcGroupBy.cpp)
//	function "convertToUnixTime" is match with "convertToTimeUnit"
///////////////////////////////////////////////////////////////////////////////
i64  
AosStatTimeUnit::convertToUnixTime(
				const i64 time_value,
				AosStatTimeUnit::E timeUnit)
{
	i64 new_time;
	i64 time_zone = timezone;
	switch (timeUnit)
	{
		case eEpochTime:
			{
				new_time = time_value;
				new_time += time_zone;
				break;
			}
		case eEpochHour:
			{
				new_time = time_value * AOSTIMEFORMAT_SECONDS_PRE_HOUR;
				new_time += time_zone;
				break;
			}
		case eEpochDay:
			{
				new_time = time_value * AOSTIMEFORMAT_SECONDS_PRE_DAY;
				new_time += time_zone;
				break;
			}
		case eEpochWeek:
			{
				new_time = time_value * 7 + 3 + 1;
				new_time = new_time * AOSTIMEFORMAT_SECONDS_PRE_DAY;
				new_time += time_zone;
				break;
			}
		case eEpochMonth:
			{
				struct tm tm;
				int year = time_value/ 12 + 1970;
				int month = time_value % 12 + 1;
				OmnString time_str,month_str;
				if (month > 9)
					    month_str << month;
				else
					    month_str << "0" << month;
				time_str << year <<"-" <<month_str << "-01 00:00:00";
				strptime(time_str.data(),"%Y-%m-%d %H:%M:%S", &tm);
				new_time = (int64_t)mktime(&tm);
				break;
			}

		case eEpochYear:
			{
				struct tm tm;
				int year = time_value + 1970;
				OmnString time_str;
				time_str << year << "-01-01 00:00:00";
				strptime(time_str.data(),"%Y-%m-%d %H:%M:%S", &tm);
				new_time = (int64_t)mktime(&tm);
				break;
			}
		default:
			{
				OmnShouldNeverComeHere;
				break;
			}
	}
	return new_time;
}

//////////////////////////////////////////////////////////////////////////////
//	JimoDB-1041
//	arvin 2015.10.26
//	this function is replaced with transformTime (function in DataProcGroupBy.cpp)
///////////////////////////////////////////////////////////////////////////////
bool	
AosStatTimeUnit::convertToTimeUnit(
				AosValueRslt &time,
				const AosStatTimeUnit::E timeUnit)
{
	//arvin 2015.11.12
	//JIMODB-1089
	aos_assert_r(time.getType() == AosDataType::eDateTime,false);
	AosDateTime dt = time.getDateTime();
	if(dt.isNotADateTime())
		return false;
	i64 epochTime = time.getI64();
	i64 new_time;
	i64 time_zone = timezone;
	epochTime -= time_zone;
	switch (timeUnit)
	{
		case AosStatTimeUnit::eEpochTime:
			{
				new_time = epochTime;
				break;
			}
		case AosStatTimeUnit::eEpochHour:
			{
				new_time = epochTime / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
				break;
			}
		case AosStatTimeUnit::eEpochDay:
			{
				new_time = epochTime / AOSTIMEFORMAT_SECONDS_PRE_DAY;
				break;
			}
		case AosStatTimeUnit::eEpochWeek:
			{
				new_time = epochTime / AOSTIMEFORMAT_SECONDS_PRE_DAY;
				new_time -= 4;
				new_time /= 7;
				break;
			}
		case AosStatTimeUnit::eEpochMonth:
			{
				struct tm t;
				gmtime_r((time_t *)&epochTime, &t);
				new_time = (t.tm_year - 70) * 12 + (t.tm_mon);
				break;
			}

		case AosStatTimeUnit::eEpochYear:
			{
				struct tm t;
				gmtime_r((time_t *)&epochTime, &t);
				new_time = t.tm_year - 70;
				break;
			}
		default:
			{
				OmnShouldNeverComeHere;
				break;
			}
		}
	time.setI64(new_time);	
	return true;
}


/*
OmnString
AosStatTimeUnit::convertToCalendarTime(const int64_t epoch_time, 
									AosStatTimeUnit::E timeUnit)
{
	OmnString timeStr = "";
	int year, month;
	bool rslt;
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosValueRslt value;
	AosTimeFormat::E targetFormat = AosTimeFormat::eYYYYMMDDHHMMSS;

	//translate the epoch_time, which is a number from 1970 
	//in terms of day, month or year, to calendar string
	switch (timeUnit)
	{
		case eEpochYear:
			timeStr << 1970 + epoch_time;
			break;

		case eEpochMonth:
			year = epoch_time / 12 + 1970;
			month = epoch_time % 12 + 1;
			
			if (month > 9)
				timeStr << year << month;
			else
				timeStr << year << "0" << month;

			break;

		case eEpochDay:
			timeStr << epoch_time;
			rslt = AosTimeFormat::convert(timeStr.data(), timeStr.length(),
				AosTimeFormat::eEpochDay, targetFormat, value, rdata);
			aos_assert_r(rslt, "");

			timeStr = value.getStr();
			break;

		default:
			timeStr = "Not Supported";
			break;
	}

	return timeStr;
}
*/


/*
int
AosStatTimeUnit::parseTimeValue(
		const int crt_epoch_time,
		const E crt_time_unit,
		const E to_time_unit)
{
	// ignore eEpochTime and eAllTime. 
	//aos_assert_r(crt_epoch_time >=0, -1);

	switch(to_time_unit)
	{
	case eEpochTime:
		return parseToEpochTime(crt_epoch_time, crt_time_unit);

	case eDay:
		return parseToEpochDay(crt_epoch_time, crt_time_unit);

	case eWeek:
		return parseToEpochWeek(crt_epoch_time, crt_time_unit);

	case eMonth:
		return parseToEpochMonth(crt_epoch_time, crt_time_unit);

	case eYear:
		return parseToEpochYear(crt_epoch_time, crt_time_unit);
	
	case eAllTime:
		return 0; 

	default:
		break;
	}
	
	OmnShouldNeverComeHere;
	return -1;
}

int
AosStatTimeUnit::parseToEpochTime(
		const int crt_epoch_time,
		const E crt_time_unit)
{
	switch(crt_time_unit)
	{
	case eEpochTime:
		return crt_epoch_time;

	case eDay:
		break;

	case eWeek:
		break;

	case eMonth:
		break;
	
	case eYear:
		break;

	case eAllTime:
		break;

	default:
		break;
	}

	OmnNotImplementedYet;
	return -1;
}


int
AosStatTimeUnit::parseToEpochDay(
		const int crt_epoch_time,
		const E crt_time_unit)
{
	switch(crt_time_unit)
	{
	case eEpochTime:
		return crt_epoch_time / AosTimeUtil::eSecondsPerDay; 
	
	case eDay:
		return crt_epoch_time;
	
	case eWeek:
		break;

	case eMonth:
		break;
	
	case eYear:
		break;

	case eAllTime:
		break;

	default:
		break;
	}

	OmnNotImplementedYet;
	return -1;
}

int
AosStatTimeUnit::parseToEpochWeek(
		const int crt_epoch_time,
		const E crt_time_unit)
{
	int epoch_time = 0;
	switch(crt_time_unit)
	{
	case eEpochTime:
		return AosTimeUtil::getEpochWeek(crt_epoch_time);
	
	case eDay:
		epoch_time = crt_epoch_time * AosTimeUtil::eSecondsPerDay; 
		return AosTimeUtil::getEpochWeek(epoch_time);

	case eWeek:
		break;

	case eMonth:
		break;
	
	case eYear:
		break;

	case eAllTime:
		break;

	default:
		break;
	}

	OmnNotImplementedYet;
	return -1;
}


int
AosStatTimeUnit::parseToEpochMonth(
		const int crt_epoch_time,
		const E crt_time_unit)
{
	int epoch_time = 0;
	switch(crt_time_unit)
	{
	case eEpochTime:
		return AosTimeUtil::getEpochMonth(crt_epoch_time);
	
	case eDay:
		epoch_time = crt_epoch_time * AosTimeUtil::eSecondsPerDay; 
		return AosTimeUtil::getEpochMonth(epoch_time);

	case eWeek:
		break;

	case eMonth:
		break;
	
	case eYear:
		break;

	case eAllTime:
		break;

	default:
		break;
	}

	OmnNotImplementedYet;
	return -1;
}


int
AosStatTimeUnit::parseToEpochYear(
		const int crt_epoch_time,
		const E crt_time_unit)
{
	int epoch_time = 0;
	switch(crt_time_unit)
	{
	case eEpochTime:
		return AosTimeUtil::getEpochYear(crt_epoch_time);
	
	case eDay:
		epoch_time = crt_epoch_time * AosTimeUtil::eSecondsPerDay; 
		return AosTimeUtil::getEpochYear(epoch_time);

	case eWeek:
		break;

	case eMonth:
		break;
	
	case eYear:
		break;

	case eAllTime:
		break;

	default:
		break;
	}

	OmnNotImplementedYet;
	return -1;
}
*/

