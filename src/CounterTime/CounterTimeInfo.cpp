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
//
// Modification History:
// 06/07/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterTime/CounterTimeInfo.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "CounterTime/TimeGranularity.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/local_time/local_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "XmlUtil/XmlTag.h"
#include <string>

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

AosCounterTimeInfo::AosCounterTimeInfo(
		const u32 year,
		const int month,
		const int day)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(0),
mMinute(0),
mSecond(0),
mMsec(0)
{
	
}


AosCounterTimeInfo::AosCounterTimeInfo(
		const u32 year,
		const int month,
		const int day,
		const int hour)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(hour),
mMinute(0),
mSecond(0),
mMsec(0)
{
}


AosCounterTimeInfo::AosCounterTimeInfo(
		const int year,
		const int month,
		const int allDay)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(hour),
mMinute(0),
mSecond(0),
mMsec(0)
{
	OmnString timeStr;
	timeStr << year << "-" << month << "-01 00:00:00.000";
	bool rslt = init(timeStr);
	aos_assert(rslt);
	aos_assert(allDay >= 1 && allDay <= 366);
	mDay = allDay - mSpendDay + 1;
}


AosCounterTimeInfo::AosCounterTimeInfo(
		const int year,
		const int month,
		const int allDay,
		const int allHour)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(hour),
mMinute(0),
mSecond(0),
mMsec(0),
mSpendDay(0),
mSpendHour(0)
{
	OmnString timeStr;
	timeStr << year << "-" << month << "-01 00:00:00.000";
	bool rslt = init(timeStr);
	aos_assert(rslt);
	aos_assert(allDay >= 1 && allDay <= 366);
	mDay = allDay - mSpendDay + 1;
	aos_assert(allHour >= 1 && allHour <= 366 * 24);
	mHour = allHour - (allDay - 1) * 24;
}


AosCounterTimeInfo::AosCounterTimeInfo(const OmnString timeStr)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(hour),
mMinute(0),
mSecond(0),
mMsec(0),
mSpendDay(0),
mSpendHour(0)
{
	bool rslt = init(timeStr);
	aos_assert(rslt);
}


AosCounterTimeInfo::AosCounterTimeInfo(const long time)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(hour),
mMinute(0),
mSecond(0),
mMsec(0),
mSpendDay(0),
mSpendHour(0)
{
	bool rslt = initWithNum(time);
	aos_assert(rslt);
}


AosCounterTimeInfo::AosCounterTimeInfo()
:
mYear(year),
mMonth(month),
mDay(day),
mHour(hour),
mMinute(0),
mSecond(0),
mMsec(0),
mSpendDay(0),
mSpendHour(0)
{
}


bool
AosCounterTimeInfo::init(const OmnString timeStr)
{
	if (timeStr != "")
	{
		try
		{
			std::string time(timeStr.data());
			ptime pt = time_from_string(time);
			date dt = pt.date();
			date::ymd_type timeYmd = dt.year_month_day();
			mYear = timeYmd.year;
			mMonth = timeYmd.month;
			mDay = timeYmd.day;
			mSpendDay = dt.day_of_year();
			
			time_duration tm = pt.time_of_day();
			mHour = tm.hours();
			mSpendHour = (mSpendDay - 1)*24 + mHour;
		}

		catch(std::exception &e)
		{
			OmnAlarm << "The time format is wrong." << timeStr << e.what()<< enderr;
			return false;
		}
	}
	else
	{
		ptime pt = second_clock::local_time();
		date today = pt.date();
		date::ymd_type timeYmd = today.year_month_day();
		mYear = timeYmd.year;
        mMonth = timeYmd.month;
        mDay = timeYmd.day;
        mSpendDay = today.day_of_year();

		time_duration tm = pt.time_of_day();
		mHour = tm.hours();
		mSpendHour = (mSpendDay - 1)*24 + mHour;

	}
	return true;
}


bool
AosCounterTimeInfo::initWithNum(const long time)
{
	// The time should be a number of seconds.
	time_t t = time;
	ptime pt = from_time_t(t);
	time_zone_ptr zone(OmnNew posix_time_zone("GMT+8"));
	local_date_time az(pt, zone);
	date dt = pt.date();

	date::ymd_type timeYmd = dt.year_month_day();
	mYear = timeYmd.year;
    mMonth = timeYmd.month;
    mDay = timeYmd.day;
    mSpendDay = dt.day_of_year();

	time_duration tm = pt.time_of_day();
	mHour = tm.hours();
	mSpendHour = (mSpendDay - 1)*24 + mHour;

	return true;
}


OmnString
AosCounterTimeInfo::createTimeCounterName(const OmnString &type)
{
	OmnString ss;
	switch (toEnum(type))
	{
	case eYear:
		 ss << mYear;
		 return ss;

	case eYearMonth:
		 ss << mMonth << AOSTAG_COUNTER_SEP << mYear;
		 return ss;

	case eYearMonthDay:
		 ss << mDay << AOSTAG_COUNTER_SEP 
			<< mMonth << AOSTAG_COUNTER_SEP 
			<< mYear;
		 return ss;

	case eYearMonthDayHour:
		 ss << mHour << AOSTAG_COUNTER_SEP
			<< mDay << AOSTAG_COUNTER_SEP 
			<< mMonth << AOSTAG_COUNTER_SEP 
			<< mYear;
		 return ss;

	case eMinute:
		 ss << mMinute << AOSTAG_COUNTER_SEP
			<< mHour << AOSTAG_COUNTER_SEP
			<< mDay << AOSTAG_COUNTER_SEP 
			<< mMonth << AOSTAG_COUNTER_SEP 
			<< mYear;
		 return ss;

	case eSecond:
		 ss << mSecond << AOSTAG_COUNTER_SEP
			<< mMinute << AOSTAG_COUNTER_SEP
			<< mHour << AOSTAG_COUNTER_SEP
			<< mDay << AOSTAG_COUNTER_SEP 
			<< mMonth << AOSTAG_COUNTER_SEP 
			<< mYear;
		 return ss;

	case eMsecond:
		 ss << mMsecond << AOSTAG_COUNTER_SEP
			<< mSecond << AOSTAG_COUNTER_SEP
			<< mMinute << AOSTAG_COUNTER_SEP
			<< mHour << AOSTAG_COUNTER_SEP
			<< mDay << AOSTAG_COUNTER_SEP 
			<< mMonth << AOSTAG_COUNTER_SEP 
			<< mYear;
		 return ss;

	case eNoTime:
		 return "";

	default:
		 OmnAlarm << "Invalid time type: " << type << enderr;
		 return "";
	}
	OmnShouldNeverComeHere;
	return "";
}


AosCounterTime::E
AosCounterTime::toEnum(const OmnString &format)
{
	if (format.length() < 1) return eInvalid;

	switch (format.data()[0])
	{
	case 'D':
	case 'd': 
		 if (format == "DDMMYYYY" || format == "ddmmyyyy") return eYear;
		 break;
	
	case 'H':
	case 'h':
		 if (format == "HHDDMMYYYY" || format == "hhddmmyyyy") return eHour;
		 break;

	case 'M':
	case 'm':
		 if (format == "MMYYYY" || format == "mmyyyy") return eMonth;
		 break;

	case 'Y':
	case 'y':
		 if (format == "YYYY" || format == "yyyy") return eYear;
		 break;

	case 'm':
	case 'M':
		 if (format == "MM" || format == "mm") return eMinute;
		 break;

	case 's':
	case 'S':
		 if (format == "SS" || format == "ss") return eSecond;
		 break;

	case 'u':
	case 'U':
		 if (format == "UU" || format == "uu") return eMsec;
		 break;

	default:
		 break;
	}

	OmnAlarm << "Unrecognized Format: " << format << enderr;
	return eInvalid;
}


u64 
AosCounterTime::getUniTime(const E time_type) const
{
	switch (time_gran)
	{
	case eYear:
		 return (mYear 	<< AosUniTime::eYearShift);

	case eYearMonth:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift);

	case eYearMonthDay:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift);

	case eYearMonthDayHour:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift) + 
			    (mHour 		<< AosUniTime::eHourShift);

	case eMinute:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift) + 
			    (mHour 		<< AosUniTime::eHourShift) + 
			    (mMinute 	<< AosUniTime::eMinuteShift);

	case eSecond:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift) + 
			    (mHour 		<< AosUniTime::eHourShift) + 
			    (mMinute 	<< AosUniTime::eMinuteShift) + 
			    (mSecond 	<< AosUniTime::eSecondShift);

	case eMsec:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift) + 
			    (mHour 		<< AosUniTime::eHourShift) + 
			    (mMinute 	<< AosUniTime::eMinuteShift) + 
			    (mSecond 	<< AosUniTime::eSecondShift) + 
			     mMsec;

	case eNoTime:
		 return 0;

	default:
		 OmnAlarm << "Invalid time type: " << time_gran << enderr;
		 return 0; 
	}

	OmnShouldNeverComeHere;
	return 0;
}


u64
AosCounterTimeInfo::getUniTime(
		const TimeType timetype, 
		const E gran, 
		const OmnString timeStr)
{
	aos_assert_r(timeStr != "", 0);

	try
	{
		std::string time(timeStr.data());
		ptime pt = time_from_string(time);
		date dt = pt.date();
		date::ymd_type timeYmd = dt.year_month_day();
		time_duration tm = pt.time_of_day();
	
		switch (gran)
		{
		case eYear:
		 	 return (timetype << AosUniTime::eTimeTypeShift) + 
				 	(timeYmd.year << AosUniTime::eYearShift);

		case eYearMonth:
		 	 return (timetype << AosUniTime::eTimeTypeShift) + 
		 	 	    (timeYmd.year << AosUniTime::eYearShift) + 
			    	(timeYmd.month << AosUniTime::eMonthShift);

		case eYearMonthDay:
		 	 return (timetype << AosUniTime::eTimeTypeShift) + 
		 	 	    (timeYmd.year << AosUniTime::eYearShift) + 
			    	(timeYmd.month << AosUniTime::eMonthShift) + 
					(timeYmd.day << AosUniTime::eDayShift);

		case eYearMonthDayHour:
		 	 return (timetype << AosUniTime::eTimeTypeShift) + 
		 	 	    (timeYmd.year << AosUniTime::eYearShift) + 
			    	(timeYmd.month << AosUniTime::eMonthShift) + 
					(timeYmd.day << AosUniTime::eDayShift);
			    	(tm.hours() << AosUniTime::eHourShift);

		case eMinute:
		 	 return (timetype << AosUniTime::eTimeTypeShift) + 
		 	 	    (timeYmd.year << AosUniTime::eYearShift) + 
			    	(timeYmd.month << AosUniTime::eMonthShift) + 
					(timeYmd.day << AosUniTime::eDayShift);
			    	(tm.hours() << AosUniTime::eHourShift);

	case eSecond:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift) + 
			    (mHour 		<< AosUniTime::eHourShift) + 
			    (mMinute 	<< AosUniTime::eMinuteShift) + 
			    (mSecond 	<< AosUniTime::eSecondShift);

	case eMsec:
		 return (mYear 		<< AosUniTime::eYearShift) + 
			    (mMonth 	<< AosUniTime::eMonthShift) + 
			    (mDay 		<< AosUniTime::eDayShift) + 
			    (mHour 		<< AosUniTime::eHourShift) + 
			    (mMinute 	<< AosUniTime::eMinuteShift) + 
			    (mSecond 	<< AosUniTime::eSecondShift) + 
			     mMsec;

	case eNoTime:
		 return 0;

	default:
		 OmnAlarm << "Invalid time type: " << time_gran << enderr;
		 return 0; 
	}

	catch(std::exception &e)
	{
		OmnAlarm << "The time format is wrong." << timeStr << e.what()<< enderr;
		return 0;
	}
	return true;
}
#endif

