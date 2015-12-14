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

#include "CounterTime/TimeCond.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "XmlUtil/XmlTag.h"
#include <string>

using namespace boost::gregorian;
using namespace boost::posix_time;

AosTimeCond::AosTimeCond(
		const OmnString startTime,
		const OmnString endTime,
		const OmnString type)
:
mStartTime(startTime),
mEndTime(endTime),
mType(type)
{
	init();
}


AosTimeCond::AosTimeCond(
		const long startTime,
		const long endTime,
		const OmnString type)
:
mType(type)
{
	initWithNum(startTime, endTime);
}


bool
AosTimeCond::init()
{
	aos_assert_r(mStartTime != "", false);
	aos_assert_r(mEndTime != "", false);
	aos_assert_r(mType != "", false);
	try
	{
		std::string startStr(mStartTime.data());
		ptime pt = time_from_string(startStr);
		date startDate = pt.date();
		date::ymd_type startYmd = startDate.year_month_day(); 

		mStartYear = startYmd.year;
		mStartMonth = startYmd.month;
		mStartDay = startYmd.day;
		mStartSpendDay = startDate.day_of_year();

		time_duration startTm = pt.time_of_day();
		mStartHour = startTm.hours();
		mStartSpendHour = (mStartSpendDay - 1)*24 + mStartHour;


		std::string endStr(mEndTime.data());
		ptime endPt = time_from_string(endStr);
		date endDate = endPt.date();
		date::ymd_type endYmd = endDate.year_month_day(); 

		mEndYear = endYmd.year;
		mEndMonth = endYmd.month;
		mEndDay = endYmd.day;
		mEndSpendDay = endDate.day_of_year();

		time_duration endTm = endPt.time_of_day();
		mEndHour = endTm.hours();
		mEndSpendHour = (mEndSpendDay - 1)*24 + mEndHour;

	}
	catch(std::exception &e)
	{
		OmnAlarm << "Exception : " << e.what() << enderr;
	}
	return true;
}


bool
AosTimeCond::initWithNum(const long startTime, const long endTime)
{
	aos_assert_r(startTime, false);
	aos_assert_r(endTime, false);
	aos_assert_r(mType != "", false);

	time_t t = startTime;
    ptime startpt = from_time_t(t);
    date startDate = startpt.date();
	time_duration starttm = startpt.time_of_day();
	date::ymd_type startYmd = startDate.year_month_day();
	mStartYear = startYmd.year;
	mStartMonth = startYmd.month;
	mStartDay = startYmd.day;
	mStartHour = starttm.hours();
	mEndSpendDay = startDate.day_of_year();
	mStartSpendHour = (mStartSpendDay - 1)*24 + mStartHour;

	t = endTime;
    ptime endpt = from_time_t(t);
    date endDate = endpt.date();
	time_duration endtm = endpt.time_of_day();
	date::ymd_type endYmd = endDate.year_month_day();
	mEndYear = endYmd.year;
	mEndMonth = endYmd.month;
	mEndDay = endYmd.day;
	mEndHour = endtm.hours();
	mEndSpendDay = endDate.day_of_year();
	mEndSpendHour = (mEndSpendDay - 1)*24 + mEndHour;

	return true;
}
