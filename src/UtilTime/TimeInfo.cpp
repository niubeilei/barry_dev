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
//	2011/04/19	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TimeInfo.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryUtil/QrUtil.h"
#include "UtilTime/TimeUtil.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include <math.h>

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

i64 AosTime::smMinYearFormat1 = 1000000;			// 1 million
i64 AosTime::smMinYearFormat2 = 10000000;			// 10 million 
i64 AosTime::smMinYearFormat3 = 30000000000000LL;	// 30 trillion

static int sgNumDaysByMonth[13] = 
{
	0, 
	31,
	59,
	90,
	120,
	151,
	181,
	212,
	243,
	273,
	304,
	334,
	365
};


AosTime::AosTime(
		const int64_t &year,
		const int month,
		const int day)
:
mYear(year),
mMonth(month),
mDay(day),
mHour(0),
mMinute(0),
mSecond(0),
mMsec(0),
mTimeFormat(eFormat2),
mTimeGran(AosTimeGran::eHourly)
{
}


AosTime::AosTime(
		const int64_t &year,
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
mMsec(0),
mTimeFormat(eFormat2),
mTimeGran(AosTimeGran::eHourly)
{
}


AosTime::AosTime(const AosTimeGran::E time_gran, const OmnString &timeStr, const OmnString bt_format)
:
mYear(-1),
mMonth(-1),
mDay(-1),
mHour(-1),
mMinute(-1),
mSecond(-1),
mMsec(-1),
mTimeFormat(eFormat2),
mTimeGran(time_gran)
{
	bool rslt = setTimeByStr(timeStr, bt_format);
	aos_assert(rslt);
}


AosTime::AosTime(const u32 time)
:
mYear(-1),
mMonth(-1),
mDay(-1),
mHour(-1),
mMinute(-1),
mSecond(-1),
mMsec(-1),
mTimeFormat(eFormat2),
mTimeGran(AosTimeGran::eHourly)
{
	bool rslt = setTimeByEpoch(time);
	aos_assert(rslt);
}


/*
OmnString
AosTime::getTimeStr(const OmnString &formatstr)
{
	time_tt = time(0);
	struct tm *theTime = localtime(&tt);
	OmnString str;

	Format format = toEnum(formatstr);
	int year = theTime->tm_year - 100 + 2000;
	char buff[50];
	switch (format)
	{
	case eMM_DD_YY:
		 sprintf(buff, "%02d-%02d-%02d", 
			theTime->tm_mon+1,
			theTime->tm_mday, 
			year);
		 return buff;

	case eMM_DD_YY_HH_MM_SS:
		 // It is the form: "dd-mm-yyyy hh:mm:ss"
		 sprintf(buff, "%02d-%02d-%02d %2d:%2d:%2d", 
			theTime->tm_mon+1,
			theTime->tm_mday, 
			year, 
			theTime->tm_hour,
			theTime->tm_min,
			theTime->tm_sec);
		 return buff;
	}
}
*/


bool
AosTime::setTimeByStr(const OmnString &timeStr, const OmnString &bt_format)
{
	// boost time format(bt_format)
	// [%m %d %Y] 05 02 2012
	// [%Y/%m/%d %H:%M:%S] 2012/05/02 00:00:00
	// [%Y%m%d] 20120502
	// [%m-%d-%Y %H:%M:%S] 05-02-2012 15:17:37
	// [%y-%m-%d %H:%M:%S] 12/05/02 00:00:00
	// [%Y-%m-%d %H:%M:%S] 2012-05-02 00:00:00
	//if (timeStr == "") return true;
	OmnString formatStr = bt_format;
	if (timeStr != "")
	{
		if (formatStr == "")
		{
			OmnAlarm << "time format is empty:" << formatStr << enderr;
			formatStr = "%Y/%m/%d %H:%M:%S";
		}

		try
		{
			//ptime pt = time_from_string(time);
			std::string time(timeStr.data());
			ptime pt(not_a_date_time);
			std::stringstream ss;
			local_time_input_facet* input_facet = OmnNew local_time_input_facet();
			ss.imbue(locale(ss.getloc(), input_facet));
			input_facet->format(formatStr.data());
			ss.str(time);
			ss >> pt;
			if (pt.is_not_a_date_time()) 
			{
				OmnAlarm << "The time format is wrong." << timeStr <<"  "<< formatStr<< enderr;
				return false;
			}

			date dt = pt.date();
			date::ymd_type timeYmd = dt.year_month_day();
			mYear = timeYmd.year;
			mMonth = timeYmd.month;
			mDay = timeYmd.day;
			
			time_duration tm = pt.time_of_day();
			mHour = tm.hours();
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

		time_duration tm = pt.time_of_day();
		mHour = tm.hours();

	}
	return true;
}


bool
AosTime::setTimeByEpoch(const i64 time)
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

	time_duration tm = pt.time_of_day();
	mHour = tm.hours();

	return true;
}


bool
AosTime::moveYear(const int delta)
{
	// It moves to the beginning of the year of 'delta' relative to
	// the current year.
	mYear += delta;
	mMonth = 0;
	mDay = 0;
	mHour = 0;
	mMinute = 0;
	mSecond = 0;
	mMsec = 0;
	return true;
}


bool
AosTime::moveMonth(const int delta)
{
	// It moves to the beginning of the month of 'delta' relative to
	// the current date.
	int num_years = delta / 12;
	mYear += num_years;
	int nn = delta % 12;
	mMonth += nn;
	if (mMonth > 12) 
	{
		mYear++;
		mMonth -= 12;
	}
	else if (mMonth < 1)
	{
		mYear--;
		mMonth += 12;
	}
	mDay = 0;
	mHour = 0;
	mMinute = 0;
	mSecond = 0;
	mMsec = 0;
	return true;
}


bool
AosTime::moveWeek(const int delta)
{
	// It moves to the beginning of the week of 'delta' relative
	// to the current date. For instance, if 'delta' is 0, it moves
	// to the beginning of the current week. If 'delta' is 1, it
	// moves to the beginning of the last week.
	bool rslt = moveDay(delta * 7);
	aos_assert_r(rslt, false);
	int weekday = getWeekday(mYear, mMonth, mDay);
	if (weekday == 1) return true;
	return moveDay(weekday - 1);
}


bool
AosTime::moveDay(const int delta)
{
	// It moves to the beginning of the day of 'delta' relative
	// to the current date.
	OmnNotImplementedYet;
	return false;
}


bool
AosTime::moveHour(const int delta)
{
	// It moves to the beginning of the hour of 'delta' relative
	// to the current time.
	OmnNotImplementedYet;
	return false;
}


bool
AosTime::moveMinute(const int delta)
{
	// It moves to the beginning of the minute of 'delta' relative
	// to the current time.
	OmnNotImplementedYet;
	return false;
}


bool
AosTime::moveSecond(const int delta)
{
	// It moves to the beginning of the second of 'delta' relative
	// to the current time.
	OmnNotImplementedYet;
	return false;
}


bool
AosTime::moveMsec(const int delta)
{
	// It moves to the beginning of the millisecond of 'delta' relative
	// to the current time.
	OmnNotImplementedYet;
	return false;
}


bool
AosTime::getTime(
		AosValueRslt &value, 
		const OmnString &time_gran, 
		const int64_t &time_delta)
{
	// This function retrieves the 'recent' time relative to current time. 
	// 'time_gran' tells the type of 'recent' and 'tag' is an XML tag that
	// contains the value that is needed to calculate the recent time. 
	//
	// Example: "Last Day"
	// 'time_gran' should be 'eDay' and 'tag' contains a value that is 
	// evaluated to '1'. 
	//
	// Example: "Last 5 days"
	// 'time_gran' should be 'eDay', and 'tag' contains a value that is
	// evaluated to '5'.
	AosTime time;
	time.getCurrentTime();

	switch (AosTimeGran::toEnum(time_gran))
	{
	case AosTimeGran::eYearly:
		 if (time_delta != 0) time.moveYear(time_delta);
		 break;

	case AosTimeGran::eMonthly:
		 if (time_delta != 0) time.moveMonth(time_delta);
		 break;

	case AosTimeGran::eDaily:
		 if (time_delta != 0) time.moveDay(time_delta);
		 break;

	case AosTimeGran::eHourly:
		 if (time_delta != 0) time.moveHour(time_delta);
		 break;

	case AosTimeGran::eMinutely:
		 if (time_delta != 0) time.moveMinute(time_delta);
		 break;

	case AosTimeGran::eSecondly:
		 if (time_delta != 0) time.moveSecond(time_delta);
		 break;

	case AosTimeGran::eWeekly:
		 if (time_delta != 0) time.moveWeek(time_delta);
		 break;

	default:
		 OmnAlarm << "Invalid recent time code: " << time_gran << enderr;
		 return false;
	}

	i64 vv = time.getUniTime();
	value.setI64(vv);
	return true;
}


int 
AosTime::getDayOfYear() const
{
	aos_assert_r(mMonth >= 1 && mMonth <= 12 && mDay >= 1 && mDay <= 31, -1);
	return getNumDaysByMonth(mYear, mMonth) + mDay;
}


int 
AosTime::getDayOfYear(const int64_t &year, const int month, const int day)
{
	aos_assert_r(month >= 1 && month <= 12 && day >= 1 && day <= 31, -1);
	return getNumDaysByMonth(year, month) + day;
}


int 
AosTime::getNumDaysByMonth(const int64_t &year, const int month)
{
	aos_assert_r(month >= 1 && month <= 12, -1);
	int nn = sgNumDaysByMonth[month];
	if (isLeapYear(year)) nn++;
	return nn;
}


int 
AosTime::getWeekday(
		const int64_t &year, 
		const int month,
		const int day)
{
	// This function calculates the weekday of the day [year, month, day].
	OmnNotImplementedYet;
	return -1;
}


bool
AosTime::getCurrentTime() 
{
	OmnNotImplementedYet;
	return false;
}


i64
AosTime::moveDay(
		const i64 uni_time, 
		const int delta,
		const AosTimeGran::E &time_gran)
{
	OmnString ss;
	ss << getYear(uni_time, eFormat2) 
		<< "/" << getMonth(uni_time, eFormat2)
		<< "/" << getDay(uni_time, eFormat2)
		<< " 00:00:00";
	ptime pt;
	date dt;
	bool rslt = convertToBoost(ss, pt, dt);
	if (!rslt) return 0;
	dt = dt + days(delta);
	if (dt.is_not_a_date()) return 0; 
	ptime p1(dt);
	if (p1.is_not_a_date_time()) return 0;
	ss = convertToStr(p1);
	return getUniTime(eFormat2, time_gran, ss, "%Y/%m/%d %H:%M:%S");
}

bool
AosTime::convertToBoost(const OmnString &ss, ptime &pt, date &dt)
{
	pt = time_from_string(ss.data());
	if (pt.is_not_a_date_time())
	{
	    OmnAlarm << "Missing time format!" << enderr;
	    return false;
	}
	dt = pt.date();
	if (dt.is_not_a_date())
	{
	    OmnAlarm << "Missing Data format!" << enderr;
	    return false; 
	}
	return true;
}

OmnString
AosTime::convertToStr(const ptime &pt)
{
	ostringstream os;
	time_facet *facet = OmnNew time_facet("%Y/%m/%d %H:%M:%S");
	if (!facet) return "";
	os.imbue(locale(cout.getloc(), facet));
	os << pt;
	OmnString ss;
	ss << os.str();
	return ss;
}


i64
AosTime::nextTime(
		const i64 &uni_time,
		AosTimeGran::E &time_gran,
		TimeFormat &timeformat,
		bool use_epoch_time)
{
	if (use_epoch_time) return uni_time + 1;
	ptime pt;
	date dt;
	bool rslt = false;
	OmnString ss;
	switch (time_gran)
	{
	case AosTimeGran::eYearly:
		 {
			 int64_t yy = getYear(uni_time, timeformat) + 1;
			 return getYearlyUniTime(yy, timeformat);
			//ss << getYear(time, timeformat) << "-01-01 00:00:00";
			//rslt = convertToBoost(ss, pt, dt);
			//if (!rslt) return 0;
			//dt = dt + years(1);
			//if (dt.is_not_a_date()) return 0; 
			//ptime p1(dt);
			//if (p1.is_not_a_date_time()) return 0;
			//ss = convertToStr(p1);
			//return getUniTime(ss, time_gran, timeformat);
		 }
		 
	case AosTimeGran::eMonthly:
		 {
			 ss << getYear(uni_time, timeformat) << "/" << getMonth(uni_time, timeformat)
			 << "/01 00:00:00";
			 rslt = convertToBoost(ss, pt, dt);
			 if (!rslt) return 0;
			 dt = dt + months(1);
			 if (dt.is_not_a_date()) return 0; 
			 ptime p1(dt);
			 if (p1.is_not_a_date_time()) return 0;
			 ss = convertToStr(p1);
			 return getUniTime(timeformat, time_gran, ss, "%Y/%m/%d %H:%M:%S");
		 }

	case AosTimeGran::eWeekly:
		//return getWeeklyUniTime();

	case AosTimeGran::eDaily:
		 {
			 ss << getYear(uni_time, timeformat) 
				<< "/" << getMonth(uni_time, timeformat)
				<< "/" << getDay(uni_time, timeformat)
				<< " 00:00:00";
			 rslt = convertToBoost(ss, pt, dt);
			 if (!rslt) return 0;
			 dt = dt + days(1);
			 if (dt.is_not_a_date()) return 0; 
			 ptime p1(dt);
			 if (p1.is_not_a_date_time()) return 0;
			 ss = convertToStr(p1);
			 return getUniTime(timeformat, time_gran, ss, "%Y/%m/%d %H:%M:%S");
		 }

	case AosTimeGran::eHourly:
		 ss << getYear(uni_time, timeformat) 
			<< "/" << getMonth(uni_time, timeformat)
			<< "/" << getDay(uni_time, timeformat)
			<< " " << getHour(uni_time, timeformat)
			<< ":00:00";
		 rslt = convertToBoost(ss, pt, dt);
		 if (!rslt) return 0;
		 pt = pt + hours(1);
		 if (pt.is_not_a_date_time()) return 0;
		 ss = convertToStr(pt);
		 return getUniTime(timeformat, time_gran, ss, "%Y/%m/%d %H:%M:%S");

	case AosTimeGran::eMinutely:
		 ss << getYear(uni_time, timeformat) 
			<< "/" << getMonth(uni_time, timeformat)
			<< "/" << getDay(uni_time, timeformat)
			<< " " << getHour(uni_time, timeformat)
			<< ":" << getMinute(uni_time, timeformat)
			<< ":00";
		 rslt = convertToBoost(ss, pt, dt);
		 if (!rslt) return 0;
		 pt = pt + minutes(1);
		 if (pt.is_not_a_date_time()) return 0;
		 ss = convertToStr(pt);
		 return getUniTime(timeformat, time_gran, ss, "%Y/%m/%d %H:%M:%S");

	case AosTimeGran::eSecondly:
		 ss << getYear(uni_time, timeformat) 
			<< "/" << getMonth(uni_time, timeformat)
			<< "/" << getDay(uni_time, timeformat)
			<< " " << getHour(uni_time, timeformat)
			<< ":" << getMinute(uni_time, timeformat)
			<< ":" << getSecond(uni_time, timeformat);
		 rslt = convertToBoost(ss, pt, dt);
		 if (!rslt) return 0;
		 pt = pt + seconds(1);
		 if (pt.is_not_a_date_time()) return 0;
		 ss = convertToStr(pt);
		 return getUniTime(timeformat, time_gran, ss, "%Y/%m/%d %H:%M:%S");

	default:
		 return 0;
	}
	return 0;
}

bool
AosTime::calculateTime(
		const OmnString &timetype,
		const int &num,
		OmnString &start_time, 
		OmnString &end_time,
		const AosRundataPtr &rdata)
{
	aos_assert_r(timetype != "", false);
	ptime crt_ptime, cal_ptime;
	AosTimeUtil::calculateTimeToPtime(num, timetype, crt_ptime, cal_ptime, rdata);
	if (crt_ptime.is_not_a_date_time() || cal_ptime.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format") << enderr;
		return false;
	}

	if (num >= 0)
	{
		start_time = AosTimeUtil::PtimeToLocalTime(rdata, crt_ptime);
		end_time = AosTimeUtil::PtimeToStr(rdata, cal_ptime);
		char c = timetype.data()[0];
		if (c == 'H'|| c == 'M' || c == 'S')
		{
			end_time  = AosTimeUtil::PtimeToLocalTime(rdata, cal_ptime);
		}
	}
	else
	{
		char c = timetype.data()[0];
		start_time = AosTimeUtil::PtimeToStr(rdata, cal_ptime);
		if (c == 'H'|| c == 'M' || c == 'S')
		{
			start_time = AosTimeUtil::PtimeToLocalTime(rdata, cal_ptime);
		}

		end_time = AosTimeUtil::PtimeToLocalTime(rdata, crt_ptime);
	}
	return true;

}


// Chen Ding, 2014/01/29
// u64 
// AosTime::convertEpochTime(
// 		const AosTimeGran::E gran,
// 		const u64 &unitime)
i64 
AosTime::convertEpochTime(
		const AosTimeGran::E gran,
		const i64 &unitime)
{
	OmnString timestr;
	i64 vv;
	OmnString formatStr = "%Y%m%d%H%M%S";
	AosTimeFormat::E format = AosTimeFormat::toEnum("yyyymmddhhmmss");
	switch (gran)
	{
	case AosTimeGran::eYearly:
		 timestr = AosTimeUtil::EpochToTimeNew(unitime, formatStr);
		 vv =  AosTimeFormat::strToEpochYear(timestr, format);
		 return vv;

	case AosTimeGran::eMonthly: 
		 timestr = AosTimeUtil::EpochToTimeNew(unitime, formatStr);
		 vv = AosTimeFormat::strToEpochMonth(timestr, format);
		 return vv;

	case AosTimeGran::eDaily:
		 timestr = AosTimeUtil::EpochToTimeNew(unitime, formatStr);
		 vv = AosTimeFormat::strToEpochDay(timestr, format);
		 return vv;

	case AosTimeGran::eHourly:
		 timestr = AosTimeUtil::EpochToTimeNew(unitime, formatStr);
		 vv = AosTimeFormat::strToEpochHour(timestr, format);
		 return vv;

	case AosTimeGran::eWeekly:
		 timestr = AosTimeUtil::EpochToTimeNew(unitime, formatStr);
		 vv = AosTimeFormat::strToEpochWeek(timestr, format);
		 return vv;

	default:
		 OmnAlarm << "Invalid time gran: " << gran << enderr;
		 return 0;
	}
	return 0;
}


OmnString 
AosTime::convertEpochToStr(
		const i64 &time,
		AosTimeGran::E time_gran)
{
	switch (time_gran)
	{
	case AosTimeGran::eDaily:
		 return AosTimeUtil::EpochToTimeNew(time * 24 * 60* 60, "%Y-%m-%d");

	case AosTimeGran::eHourly:
		 return AosTimeUtil::EpochToTimeNew(time * 60 * 60, "%Y-%m-%d %H:%M:%S");

	default:
		 OmnAlarm << "Invalid time gran: " << time_gran << enderr;
	 	 return "";
	}
	return "";
}

