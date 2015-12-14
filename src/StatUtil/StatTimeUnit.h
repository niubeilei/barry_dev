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
// 2014/02/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatUtil_StatTimeUnit_h
#define Aos_StatUtil_StatTimeUnit_h

#include "Util/Ptrs.h"
#include "UtilTime/TimeUtil.h"

#define AOS_TIMEUNIT_EPOCHTIME	"_epoch_time"

#define AOS_TIMEUNIT_HOUR		"_hour"
#define AOS_TIMEUNIT_DAY		"_day"
#define AOS_TIMEUNIT_WEEK		"_week"
#define AOS_TIMEUNIT_MONTH		"_month"
#define AOS_TIMEUNIT_YEAR		"_year"

#define AOS_TIMEUNIT_ALLTIME	"_alltime"		// internal feature.

struct AosStatTimeUnit
{
	enum E
	{
		eEpochTime,
		
		eEpochHour,
		eEpochDay,
		eEpochWeek,
		eEpochMonth,
		eEpochYear,
		eAllTime,

		eInvalid,
	};
	
	enum 
	{
		eSecondsPerHour = 60 * 60,
		eHoursPerDay = 12,
	};

	typedef int (*ParseFun)(int);

private:
	static ParseFun 	smParseFuncs[eInvalid][eInvalid];
	static bool			smInited;

public:
	static void init();

	static E getTimeUnit(const OmnString &field);
	static bool isValid(const E cond) { return cond < eInvalid;	};
	static bool isTimeUnitStr(const OmnString &str);
	static OmnString toStr(const E cond);
	
	static int parseTimeValue(
				const int crt_epoch_time,
				const E crt_time_unit,
				const E parser_unit);
	
	static void KettyInitPrintTime();
	static void KettyPrintTime();
	static OmnString convertToCalendarTime(
				const int64_t epoch_time, 
				AosStatTimeUnit::E timeUnit);
	
	static i64  convertToUnixTime(
				const i64 time_value,
				AosStatTimeUnit::E timeUnit);
	
	static bool	convertToTimeUnit(
				AosValueRslt &time,
				const AosStatTimeUnit::E timeUnit);

	static bool isCompatible(const E left_cond, const E right_cond);

private:
	static int 	epochTimeToEpochTime(const int crt_epoch_time);
	
	static int 	epochTimeToEpochHour(const int crt_epoch_time);
	static int 	epochHourToEpochHour(const int crt_epoch_time);

	static int 	epochTimeToEpochDay(const int crt_epoch_time);
	static int 	epochHourToEpochDay(const int crt_epoch_time);
	static int 	epochDayToEpochDay(const int crt_epoch_time);

	static int 	epochTimeToEpochWeek(const int crt_epoch_time);
	static int 	epochHourToEpochWeek(const int crt_epoch_time);
	static int 	epochDayToEpochWeek(const int crt_epoch_time);
	static int 	epochWeekToEpochWeek(const int crt_epoch_time);
	
	static int 	epochTimeToEpochMonth(const int crt_epoch_time);
	static int 	epochHourToEpochMonth(const int crt_epoch_time);
	static int 	epochDayToEpochMonth(const int crt_epoch_time);
	static int 	epochWeekToEpochMonth(const int crt_epoch_time);
	static int  epochMonthToEpochMonth(const int crt_epoch_time);
	static int  epochYearToEpochYear(const int crt_epoch_time);

	static int 	epochTimeToEpochYear(const int crt_epoch_time);
	static int 	epochHourToEpochYear(const int crt_epoch_time);
	static int 	epochDayToEpochYear(const int crt_epoch_time);
	static int 	epochWeekToEpochYear(const int crt_epoch_time);

	static int 	parseToAllTime(const int crt_epoch_time);

	/*
	static int parseToEpochTime(
				const int crt_epoch_time,
				const E crt_time_unit);

	static int parseToEpochDay(
				const int crt_epoch_time,
				const E crt_time_unit);

	static int parseToEpochWeek(
				const int crt_epoch_time,
				const E crt_time_unit);
	
	static int parseToEpochMonth(
				const int crt_epoch_time,
				const E crt_time_unit);
	
	static int parseToEpochYear(
				const int crt_epoch_time,
				const E crt_time_unit);
	*/
};


#endif

