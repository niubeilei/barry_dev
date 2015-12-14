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
// Time can be expressed in the following integral format:
// 			year	24 bits, 
// 			month	4 bits	
// 			day		9 bits	
// 			hour	5 bits	
// 			minute  6 bits	
// 			second 	6 bits	
// 			msecond	10 bits
//
// Modification History:
// 06/07/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_COunterTime_TimeUtil_h
#define Omn_COunterTime_TimeUtil_h

#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "CounterTime/Ptrs.h"


class AosTimeUtil : public OmnRCObject
{
	OmnDefineRCObject;
public:
	enum
	{
		eYearShift       = 40
		eMonthShiftmonth = 36
		eDayShift	 	 = 27
		eHourShift	 	 = 22
		eMinuteShift 	 = 16
		eSecondShift 	 = 10
		eMsecShift		 = 0
	};

public:
	AosTimeUtil(){}
	~AosTimeUtil() {}

	inline static u64 getUniTime(
			const u32 year,
			const u32 month,
			const u32 day,
			const u32 hour,
			const u32 minute,
			const u32 second,
			const u32 msec)
	{
		return (year << eYearShift) + 
			   (month << eMonthShift) + 
			   (day << eDayShift) + 
			   (hour << eHourShift) + 
			   (minute << eMinuteShift) + 
			   (second << eSecondShift) + 
			   msec;
	}

	inline static u64 getUniTime(
			const u32 year,
			const u32 month,
			const u32 day,
			const u32 hour,
			const u32 minute,
			const u32 second)
	{
		return (year << eYearShift) + 
			   (month << eMonthShift) + 
			   (day << eDayShift) + 
			   (hour << eHourShift) + 
			   (minute << eMinuteShift) + 
			   (second << eSecondShift);
	}

	inline static u64 getUniTime(
			const u32 year,
			const u32 month,
			const u32 day,
			const u32 hour,
			const u32 minute)
	{
		return (year << eYearShift) + 
			   (month << eMonthShift) + 
			   (day << eDayShift) + 
			   (hour << eHourShift) + 
			   (minute << eMinuteShift);
	}

	inline static u64 getUniTime(
			const u32 year,
			const u32 month,
			const u32 day,
			const u32 hour)
	{
		return (year << eYearShift) + 
			   (month << eMonthShift) + 
			   (day << eDayShift) + 
			   (hour << eHourShift);
	}

	inline static u64 getUniTime(
			const u32 year,
			const u32 month,
			const u32 day)
	{
		return (year << eYearShift) + 
			   (month << eMonthShift) + 
			   (day << eDayShift);
	}

	inline static u64 getUniTime(
			const u32 year,
			const u32 month)
	{
		return (year << eYearShift) + 
			   (month << eMonthShift);
	}

	inline static getYear(const u64 &uni_time)
	{
		return uni_time >> eYearShift;
	}

	inline static getMonth(const u64 &uni_time)
	{
		return uni_time >> eYearShift;
	}

	inline static u64 getUniTime(const u32 year)
	{
		return (year << eYearShift);
	}

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

	static bool isLeapYear(const int year)
	{
		return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
	}
};
#endif
#endif
