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
//	07/29/2011 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_TimeInfo_h
#define Omn_UtilTime_TimeInfo_h

#include "aosUtil/Types.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "QueryUtil/QrUtil.h"
#include "Util/String.h"
#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/Ptrs.h"
#include "Rundata/Rundata.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/local_time/local_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;


#define AOSTIMEFORMAT_FORMAT1			"1fmt"
#define AOSTIMEFORMAT_FORMAT2			"2fmt"
#define AOSTIMEFORMAT_FORMAT3			"3fmt"

class AosValueRslt;

class AosTime
{
public:
	enum
	{
		eReservedByte	= 0,

		eFormatMask		= 0xff,			// 8 bits, reserved
		eTimeGranMask	= 0x1f,			// 5 bits
		eMonthMask		= 0xf,			// 4 bits
		eDayMask		= 0x1f,			// 5 bits
		eHourMask		= 0x1f,			// 5 bits
		eMinuteMask		= 0x3f,			// 6 bits
		eSecondMask		= 0x3f,			// 6 bits
		eMsecMask		= 0x3ff,		// 10 bits

		eMsecBits		= 10,
		eSecondBits		= 6,
		eMinuteBits		= 6,
		eHourBits		= 5,
		eDayBits		= 5,
		eMonthBits		= 4,
		eTimeGranBits 	= 5,

		eTimeGranShift	= 51,

		// Shifts for Format 2
		// 		Second		6 bits
		// 		Minute		6 bits
		// 		Hour		5 bits
		// 		Day			5 bits
		// 		Month		4 bits
		// 		Year		25 bits
		// 		Time Gran	5 bits
		// 		Reserved	8 bits
		eYearBits2		= 25,
		eYearMask2		= 0x1ffffff,	// 25 bits

		eSecondU64Mask2		= 0x7ffffffffffffULL,
		eMinuteU64Mask2		= 0x7ffffffffffc0ULL,
		eHourU64Mask2		= 0x7fffffffff000ULL,
		eDayU64Mask2		= 0x7fffffffe0000ULL,
		eMonthU64Mask2		= 0x7ffffffc00000ULL,
		eYearTruncateMask2	= 0x7fffffc000000ULL,

		eInvalidTime		= 0x00ffffffffffffffULL,

		eSecondShift2 	= 0,							// 0
		eMinuteShift2 	= eSecondBits,					// 6
		eHourShift2	 	= eMinuteShift2 + eMinuteBits,	// 12
		eDayShift2	 	= eHourShift2 + eHourBits,		// 17
		eMonthShift2	= eDayShift2 + eDayBits,		// 22
		eYearShift2     = eMonthShift2 + eMonthBits,	// 26
	};

	enum TimeFormat
	{
		eInvalidTimeFormat,

		eFormat1,
		eFormat2, 
		eFormat3,

		eMaxTimeFormat
	};

	static i64 smMinYearFormat1;
	static i64 smMinYearFormat2;
	static i64 smMinYearFormat3;

private:
	int64_t			mYear;
	int				mMonth;
	int				mDay;
	int				mHour;
	int				mMinute;
	int				mSecond;
	int				mMsec;
	TimeFormat		mTimeFormat;
	AosTimeGran::E 	mTimeGran;

public:
	AosTime()
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
	}

	AosTime(const TimeFormat format, const AosTimeGran::E gran)
	:
	mYear(-1),
	mMonth(-1),
	mDay(-1),
	mHour(-1),
	mMinute(-1),
	mSecond(-1),
	mMsec(-1),
	mTimeFormat(format),
	mTimeGran(gran)
	{
	}

	AosTime(const AosTimeGran::E gran)
	:
	mYear(-1),
	mMonth(-1),
	mDay(-1),
	mHour(-1),
	mMinute(-1),
	mSecond(-1),
	mMsec(-1),
	mTimeFormat(eFormat2),
	mTimeGran(gran)
	{
	}

	AosTime(const int64_t &year, const int month, const int day);
	AosTime(const int64_t &year, const int month, const int day, const int hour);
	AosTime(const AosTimeGran::E time_gran, const OmnString &timestr, const OmnString bt_format);
	AosTime(const u32 epoch);
	~AosTime()
	{
	}

	void setYear(const int64_t &y) {mYear = y;}
	void setMonth(int m) {mMonth = m;}
	void setDay(int m) {mDay = m;}
	void setHour(int m) {mHour = m;}
	void setMinute(int m) {mMonth = m;}
	void setSecond(int m) {mSecond = m;}
	void setMsec(int m) {mMsec = m;}

	int64_t getYear() const {return mYear;}
	int getMonth() const {return mMonth;}
	int getDay() const {return mDay;}
	int getHour() const {return mHour;}
	int getMinute() const {return mMinute;}
	int getSecond() const {return mSecond;}
	int getMsec() const {return mMsec;}

	bool isSameYear(const AosTime &rhs) const
	{
		return (mYear == rhs.mYear);
	}

	bool isSameMonth(const AosTime &rhs) const
	{
		return (mYear == rhs.mYear && mMonth == rhs.mMonth);
	}

	bool isSameDay(const AosTime &rhs) const
	{
		return (mYear == rhs.mYear && mMonth == rhs.mMonth && mDay == rhs.mDay);
	}

	bool operator <= (const AosTime &rhs) const
	{
		if (mYear > rhs.mYear) return false;

		if (mMonth == -1 || rhs.mMonth == -1) return true;
		if (mMonth > rhs.mMonth) return false;

		if (mDay == -1 || rhs.mDay == -1) return true;
		if (mDay > rhs.mDay) return false;

		if (mHour == -1 || rhs.mHour == -1) return true;
		if (mHour > rhs.mHour) return false;

		if (mMinute == -1 || rhs.mMinute == -1) return true;
		if (mMinute > rhs.mMinute) return false;

		if (mSecond == -1 || rhs.mSecond == -1) return true;
		if (mSecond > rhs.mSecond) return false;

		if (mMsec == -1 || rhs.mMsec == -1) return true;
		return mMsec <= rhs.mMsec;
	}

	bool operator < (const AosTime &rhs) const
	{
		if (mYear >= rhs.mYear) return false;

		if (mMonth == -1 || rhs.mMonth == -1) return true;
		if (mMonth >= rhs.mMonth) return false;

		if (mDay == -1 || rhs.mDay == -1) return true;
		if (mDay >= rhs.mDay) return false;

		if (mHour == -1 || rhs.mHour == -1) return true;
		if (mHour >= rhs.mHour) return false;

		if (mMinute == -1 || rhs.mMinute == -1) return true;
		if (mMinute >= rhs.mMinute) return false;

		if (mSecond == -1 || rhs.mSecond == -1) return true;
		if (mSecond >= rhs.mSecond) return false;

		if (mMsec == -1 || rhs.mMsec == -1) return true;
		return mMsec < rhs.mMsec;
	}

	inline bool nextDay(const AosTime &rhs)
	{
		// It increments this class to the next day.
		return incrementDay();
	}

	static bool isLeapYear(const int64_t &year)
	{
		// It is a leap mYear if:
		// 	1. It is evenly divisible by 4, and
		// 	2. If it can be evenly divided by 100, it is NOT a leap mYear unless
		// 	   it is evenly divisble by 400. Then it is a leap mYear.
		if (year % 4) return false;	// Not divided by 4	

		if ((year % 100) == 0)
		{
			if ((year % 400) == 0) return true;
			return false;
		}
		return true;
	}

	bool incrementDay()
	{
		// This function increments the mDay. 
		switch (mMonth)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
			 if (mDay < 31) {mDay++; return true;}
			 mDay = 1; mMonth++; 
			 return true;

		case 2:
			 if (mDay < 28) {mDay++; return true;}
			 if (mDay == 29)
			 {
				 mMonth = 3;
				 mDay = 1;
				 return true;
			 }
			 aos_assert_r(mDay == 28, false);
			 if (isLeapYear(mYear)) {mDay++; return true;}
			 mMonth = 3;
			 mDay = 1;
			 return true;

		case 4:
		case 6:
		case 9:
		case 11:
			 if (mDay < 30) {mDay++; return true;}
			 mDay = 1; mMonth++;
			 return true;

		case 12:
			 if (mDay < 31) {mDay++; return true;}
			 mYear++; mMonth = 1; mDay = 1;
			 return true;

		default:
			 OmnAlarm << "Incorrect mMonth: " << mMonth << enderr;
			 return false;
		}

		OmnShouldNeverComeHere;
		return false;
	}

	bool moveYear(const int delta);
	bool moveQuarter(const int delta);
	bool moveMonth(const int delta);
	bool moveWeek(const int delta);
	bool moveDay(const int delta);
	bool moveHour(const int delta);
	bool moveMinute(const int delta);
	bool moveSecond(const int delta);
	bool moveMsec(const int delta);
	void setInvalid() {mTimeGran = AosTimeGran::eInvalid;}
	bool isValid() const {return AosTimeGran::isValid(mTimeGran);}

	inline static i64 getUniTime(
						const TimeFormat format, 
						const AosTimeGran::E gran, 
						const OmnString &timeStr,
						const OmnString &bt_format)
	{
		if (timeStr == "") return eInvalidTime;

		AosTime time(format, gran);
		time.setTimeByStr(timeStr, bt_format);
		return time.getUniTime();
	}

	static i64 convertEpochTime(
			const AosTimeGran::E gran,
			const i64 &unitime);

	inline static i64 convertUniTime(
						const TimeFormat format, 
						const AosTimeGran::E gran, 
						const i64 &unitime,
						bool use_epoch_time)
	{
		if (use_epoch_time) return convertEpochTime(gran, unitime);

		switch (format)
		{
		case eFormat2:
			 return convertUniTimeFormat2(gran, unitime);

		case eFormat3:
			 return convertUniTimeFormat3(gran, unitime);

		case eFormat1:
		default:
			 return convertUniTimeFormat1(gran, unitime);
		}
	}

	inline static i64 convertUniTimeFormat3(
						const AosTimeGran::E gran, 
						const i64 &unitime)
	{
		OmnNotImplementedYet;
		return 0;
	}

	inline static i64 convertUniTimeFormat1(
						const AosTimeGran::E gran, 
						const i64 &unitime)
	{
		OmnNotImplementedYet;
		return 0;
	}


	inline static i64 convertUniTimeFormat2(
						const AosTimeGran::E gran, 
						const i64 &unitime)
	{
		i64 vv = ((i64)AosTimeGran::eYearly << eTimeGranShift);
		switch (gran)
		{
		case AosTimeGran::eYearly:
			 vv += (unitime & eYearTruncateMask2);
			 return vv;

		case AosTimeGran::eMonthly:
			 vv += (unitime & eMonthU64Mask2);
			 return vv;

		case AosTimeGran::eWeekly:
			 vv += (unitime & eDayU64Mask2);
			 return vv;

		case AosTimeGran::eDaily:
			 vv += (unitime & eDayU64Mask2);
			 return vv;

		case AosTimeGran::eHourly:
			 vv += (unitime & eHourU64Mask2);
			 return vv;

		case AosTimeGran::eMinutely:
			 vv += (unitime & eMinuteU64Mask2);
			 return vv;

		case AosTimeGran::eSecondly:
			 vv += (unitime & eSecondU64Mask2);
			 return vv;

		default:
			 OmnAlarm << "Invalid time gran: " << gran << enderr;
			 return 0;
		}
	}

	inline static i64 getUniTime(const OmnString timeStr, const OmnString bt_format) 
	{
		AosTime tt;
		tt.setTimeByStr(timeStr, bt_format);
		return tt.getUniTime();
	}

	i64 getUniTime() const
	{
		switch (mTimeGran)
		{
		case AosTimeGran::eYearly:
			 return getYearlyUniTime();

		case AosTimeGran::eMonthly:
			 return getMonthlyUniTime();

		case AosTimeGran::eWeekly:
			 return getWeeklyUniTime();

		case AosTimeGran::eDaily:
			 return getDailyUniTime();

		case AosTimeGran::eHourly:
			 return getHourlyUniTime();

		case AosTimeGran::eMinutely:
			 return getMinutelyUniTime();

		case AosTimeGran::eSecondly:
			 return getSecondlyUniTime();

		default:
			 return 0;
		}
	}

	bool setTimeByStr(const OmnString &timeStr, const OmnString &bt_format);
	bool setTimeByEpoch(const i64 time);
	int getDayOfYear() const;
	int getWeekday(const int64_t &year, const int month, const int day);

	static bool getTime(
				AosValueRslt &value, 
				const OmnString &time_gran, 
				const int64_t &delta);

	static bool getTime(
				AosTime &time, 
				const OmnString &time_gran, 
				const int64_t &delta);

	bool getCurrentTime();
	static int getNumDaysByMonth(const int64_t &year, const int month);
	static int getDayOfYear(const int64_t &year, const int month, const int day);

	static int getDaysByYearAndMonth(const int year, const int month)
	{
		int day = 0;
		if (month == 2)
		{
			day = isLeapYear(year) ? 29 : 28;
		}
		else if (month == 4 || month == 6 || month == 9 || month == 11)
		{
			day = 30;
		}
		else 
		{
			day = 31;
		}
		return day;
	}

	inline static i64 convertYearToInt64(const TimeFormat format, const int64_t &year)
	{
		switch (format)
		{
		case eFormat1:
			 // The minimum year is eMimYear1.
			 aos_assert_r(year >= -(int64_t)smMinYearFormat1, 0);
			 return year + smMinYearFormat1;

		case eFormat2:
			 aos_assert_r(year >= -(int64_t)smMinYearFormat2, 0);
			 return year + smMinYearFormat2;

		case eFormat3:
			 aos_assert_r(year >= -(int64_t)smMinYearFormat3, 0);
			 return year + smMinYearFormat3;

		default:
			 OmnAlarm << "Invalid time format: " << format << enderr;
			 return 0;
		}
	}

	inline static int64_t convertYearToInt64(const TimeFormat format, const int year)
	{
		switch (format)
		{
		case eFormat1:
			 return year - smMinYearFormat1;

		case eFormat2:
			 return year - smMinYearFormat2;

		case eFormat3:
			 return year - smMinYearFormat3;

		default:
			 OmnAlarm << "Invalid time format: " << format << enderr;
			 return 0;
		}
	}

	inline static i64 getUniTime(
			const int year,
			const int month,
			const int day,
			const int hour,
			const int minute,
			const int second,
			const int msec, 
			const TimeFormat format, 
			const AosTimeGran::E time_gran)
	{
		switch (time_gran)
		{
		case AosTimeGran::eYearly:
			 return getYearlyUniTime(year, format);

		case AosTimeGran::eMonthly:
			 return getMonthlyUniTime(year, month, format);

		case AosTimeGran::eWeekly:
			 return getWeeklyUniTime(year, month, day, format);

		case AosTimeGran::eDaily:
			 return getDailyUniTime(year, month, day, format);

		case AosTimeGran::eHourly:
			 return getHourlyUniTime(year, month, day, hour, format);

		case AosTimeGran::eMinutely:
			 return getMinutelyUniTime(year, month, day, hour, minute, format);

		case AosTimeGran::eSecondly:
			 return getSecondlyUniTime(year, month, day, hour, minute, second, format);

		default:
			 break;
		}

		OmnAlarm << "Unrecognized time gran: " << time_gran << enderr;
		return 0;
	}

	inline i64 getYearlyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + (yy << eYearShift2);

		default:
			 break;
		}
		OmnAlarm << "Unrecognized time format: " << mTimeFormat << enderr;
		return 0;
	}

	inline static i64 getYearlyUniTime(
				const int64_t &year,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + (yy << eYearShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline i64 getMonthlyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		int month = mMonth;
		if (month <= 0 || month > 12) month = 1;
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << (int)mTimeFormat << enderr;
			 return 0;
		}
	}

	inline static i64 getMonthlyUniTime(
				const int64_t &year,
				int month,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		if (month <= 0 || month > 12) month = 1;
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline i64 getWeeklyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		int month = mMonth;
		int day = mDay;
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline static i64 getWeeklyUniTime(
				const int64_t &year,
				int month,
				int day,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline i64 getDailyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		int month = mMonth;
		int day = mDay;
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline static i64 getDailyUniTime(
				const int64_t &year,
				int month,
				int day,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline i64 getHourlyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		int month = mMonth;
		int day = mDay;
		int hour = mHour;
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		if (hour < 0 || hour >= 24) hour = 0;
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2) + 
					((i64)hour << eHourShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline static i64 getHourlyUniTime(
				const int64_t &year,
				int month,
				int day,
				int hour,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		if (hour < 0 || hour >= 24) hour = 0;
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2) + 
					((i64)hour << eHourShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline i64 getMinutelyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		int month = mMonth;
		int day = mDay;
		int hour = mHour;
		int minute = mMinute;
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		if (hour < 0 || hour >= 24) hour = 0;
		if (minute < 0 || minute > 59) minute = 0;
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2) + 
					((i64)hour << eHourShift2) + 
					((i64)minute << eMinuteShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline static i64 getMinutelyUniTime(
				const int64_t &year,
				int month,
				int day,
				int hour,
				int minute,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		if (hour < 0 || hour >= 24) hour = 0;
		if (minute < 0 || minute > 59) minute = 0;
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2) + 
					((i64)hour << eHourShift2) + 
					((i64)minute << eMinuteShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline i64 getSecondlyUniTime() const
	{
		i64 yy = convertYearToInt64(mTimeFormat, mYear);
		int month = mMonth;
		int day = mDay;
		int hour = mHour;
		int minute = mMinute;
		int second = mSecond;
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		if (hour < 0 || hour >= 24) hour = 0;
		if (minute < 0 || minute > 59) minute = 0;
		if (second < 0 || second > 59) second = 0;
		switch (mTimeFormat)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2) + 
					((i64)hour << eHourShift2) + 
					((i64)minute << eMinuteShift2) + 
					((i32)second << eSecondShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline static i64 getSecondlyUniTime(
				const int64_t &year,
				int month,
				int day,
				int hour,
				int minute,
				int second,
				const TimeFormat format)
	{
		i64 yy = convertYearToInt64(format, year);
		if (month <= 0 || month > 12) month = 1;
		if (day <= 0 || day > 31) day = 1;
		if (hour < 0 || hour >= 24) hour = 0;
		if (minute < 0 || minute > 59) minute = 0;
		if (second < 0 || second > 59) second = 0;
		switch (format)
		{
		case eFormat2:
			 return ((i64)AosTimeGran::eYearly << eTimeGranShift) + 
				 	(yy << eYearShift2) + 
					((i64)month << eMonthShift2) + 
					((i64)day << eDayShift2) + 
					((i64)hour << eHourShift2) + 
					((i64)minute << eMinuteShift2) + 
					((i32)second << eSecondShift2);

		default:
			 OmnAlarm << "Format Not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline static i64 getYear(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		i64 yy;
		switch (format)
		{
		case eFormat2:
			 yy = (uni_time >> eYearShift2) & eYearMask2;
			 return convertYearToInt64(format, yy);

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline static int getMonth(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		switch (format)
		{
		case eFormat2:
			 return (uni_time >> eMonthShift2) & eMonthMask;

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline static int getDay(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		switch (format)
		{
		case eFormat2:
			 return (uni_time >> eDayShift2) & eDayMask;

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}

	static int getHour(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		switch (format)
		{
		case eFormat2:
			 return (uni_time >> eHourShift2) & eHourMask;

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}

	static int getMinute(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		switch (format)
		{
		case eFormat2:
			 return (uni_time >> eMinuteShift2) & eMinuteMask;

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}

	static int getSecond(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		switch (format)
		{
		case eFormat2:
			 return (uni_time >> eSecondShift2) & eSecondMask;

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}
	static int getMsec(const i64 &uni_time, const TimeFormat format = eFormat2)
	{
		switch (format)
		{
		case eFormat2:
			 return 0;

		default:
			 OmnAlarm << "Format not recognized: " << format << enderr;
			 return 0;
		}
	}

	inline int getYear(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return unitime >> eYearShift2;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline int getMonth(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return (unitime>> eMonthShift2) & eMonthMask;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline int getDay(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return (unitime >> eDayShift2) & eDayMask;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline int getHour(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return (unitime >> eHourShift2) & eHourMask;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline int getMinute(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return (unitime >> eMinuteShift2) & eMinuteMask;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline int getSecond(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return (unitime >> eSecondShift2) & eSecondMask;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	inline int getMsec(const i64 &unitime) const 
	{
		switch (mTimeFormat)
		{
		case eFormat2:
			 return 0;

		default:
			 OmnAlarm << "Format not recognized: " << mTimeFormat << enderr;
			 return 0;
		}
	}

	static bool getLastYear(AosValueRslt &value);
	static TimeFormat convertTimeFormatWithDft(const OmnString &name)
	{
		int len = name.length();
		if (len < 3) return eFormat2;
		const char *data = name.data();
		switch (data[0])
		{
		case '1':
			 if (name == AOSTIMEFORMAT_FORMAT1) return eFormat1;
			 break;

		case '2':
			 if (name == AOSTIMEFORMAT_FORMAT1) return eFormat2;
			 break;

		case '3':
			 if (name == AOSTIMEFORMAT_FORMAT1) return eFormat3;
			 break;

		default:
			 break;
		}
		return eFormat2;
	}	

	inline static bool isValidTime(const i64 &time) {return time != eInvalidTime;}
	inline static bool isValidTimeRange(const i64 &starttime, const i64 &endtime) 
	{
		return starttime != eInvalidTime && endtime != eInvalidTime && starttime <= endtime;
	}


	static OmnString convertEpochToStr(
			const i64 &time,
			AosTimeGran::E time_gran);

	inline static OmnString convertToStr(
			const i64 &time, 
			AosTimeGran::E time_gran, 
			TimeFormat &timeformat,
			bool use_epochtime)
	{
		if (use_epochtime) return convertEpochToStr(time, time_gran);
		OmnString ss;
		switch (time_gran)
		{
		case AosTimeGran::eYearly:
			 ss << getYear(time, timeformat);
			 return ss;

		case AosTimeGran::eMonthly:
			 ss << getYear(time, timeformat) << "-" << getMonth(time, timeformat);
			 return ss;

		case AosTimeGran::eWeekly:
			//return getWeeklyUniTime();

		case AosTimeGran::eDaily:
			 ss << getYear(time, timeformat) 
				<< "-" << getMonth(time, timeformat)
				<< "-" << getDay(time, timeformat);
			 return ss;

		case AosTimeGran::eHourly:
			 ss << getYear(time, timeformat) 
				<< "-" << getMonth(time, timeformat)
				<< "-" << getDay(time, timeformat)
				<< " " << getHour(time, timeformat);
			 return ss;

		case AosTimeGran::eMinutely:
			 ss << getYear(time, timeformat) 
				<< "-" << getMonth(time, timeformat)
				<< "-" << getDay(time, timeformat)
				<< " " << getHour(time, timeformat)
				<< ":" << getMinute(time, timeformat);
			 return ss;

		case AosTimeGran::eSecondly:
			 ss << getYear(time, timeformat) 
				<< "-" << getMonth(time, timeformat)
				<< "-" << getDay(time, timeformat)
				<< " " << getHour(time, timeformat)
				<< ":" << getMinute(time, timeformat)
				<< ":" << getSecond(time, timeformat);
			 return ss;

		default:
			return "";
		}
		return "";
	}

	static i64 nextTime(
			const i64 &uni_time, 
			AosTimeGran::E &time_gran, 
			TimeFormat &timeformat,
			bool use_epoch_time);

//	AosTime time(timeformat, time_gran);
//	time.setTimeByStr(ss);
//	time.moveYear(1);
//	int64_t yy = getYear(uni_time, timeformat) + 1;
//	return getYearlyUniTime(yy, timeformat);

	//	int64_t vv;	
	//	convert to boost;
	//	boost++;
	//	convert back;
	//	return;
	//	switch (time_gran)
	//	{
	//	case AosTimeGran::eYearly:
	//		 vv = getYear(time, timeformat); 
	//		 vv++;
	//		 return vv; 
	//	}

	inline static TimeFormat getDftTimeFormat() {return eFormat2;}
	inline static bool convertToBoost(
				const OmnString &ss,
				ptime &pt, 
				date &dt); 
	inline static OmnString convertToStr(const ptime &pt);

	static bool calculateTime(
			const OmnString &timetype, 
			const int &num, 
			OmnString &start_time, 
			OmnString &end_time,
			const AosRundataPtr &rdata);

	static i64 moveDay(
			const i64 uni_time,
			const int delta,
			const AosTimeGran::E &time_gran);

	// 06/06/2012, Linda
	inline static i64 getUniTime(const OmnString &timestr, const AosTimeFormat::E format, const AosTimeGran::E time_gran)
	{
		switch (time_gran)
		{
		case AosTimeGran::eDaily:
			 return AosTimeFormat::strToEpochDay(timestr, format);
		case AosTimeGran::eHourly:
			 return AosTimeFormat::strToEpochHour(timestr, format);
		case AosTimeGran::eSecondly:
			 return AosTimeFormat::strToEpochTime(timestr, format);
		default:
			 return 0;
		}
		return 0;
	}
};
#endif
