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
#include "CounterTime/TimeUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlUtil/XmlTag.h"

/*
bool
AosTimeUtil::convertTime(
		const OmnString &timestr,
		int &year, 
		int &month,
		int &day, 
		int &dayofyear,
		int &hour,
		int &minute,
		int &second, 
		int &msec)
{
	try
	{
		std::string startStr(timestr.data());
		ptime pt = time_from_string(startStr);
		date startDate = pt.date();
		date::ymd_type startYmd = startDate.year_month_day(); 

		year = startYmd.year;
		month = startYmd.month;
		day = startYmd.day;
		dayofyear = startDate.day_of_year();

		time_duration startTm = pt.time_of_day();
		hour = startTm.hours();
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
*/
