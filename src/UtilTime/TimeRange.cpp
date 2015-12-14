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
// 04/01/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TimeRange.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/local_time/local_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include <math.h>

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

bool 
AosTimeRange::getTime(
		const AosCalendarPtr &calendar, 
		const OmnString &timestr, 
		const OmnString &timeformat, 
		AosTimeRange &timeinfo,
		const int start_time, 
		const int stop_time, 
		const int begin_time, 
		const int end_time, 
		const AosRundataPtr &rdata)
{
	/*
	// This function creates the time based on:
	// 	[calendar, timestr, timeformat, start_slot, end_slot, max_slots]
	bool rslt = AosTimeRangeUtil::getTime(timestr, timeformat, timeinfo);
	aos_assert_rr(rslt, rdata, false);
	return timeinfo.setTime(begin_time, end_time, start_time, stop_time, rdata);
	*/
	OmnNotImplementedYet;
	return false;
}



AosTimeRange::AosTimeRange(
		const OmnString &startTime,
		const OmnString &endTime)
{
	OmnNotImplementedYet;
	//mStartTime.setTimeByStr(startTime);
	//mEndTime.setTimeByStr(endTime);
}


AosTimeRange::AosTimeRange(
		const u64 &startTime,
		const u64 &endTime)
{
	mStartTime.setTimeByEpoch(startTime);
	mEndTime.setTimeByEpoch(endTime);
}


AosTimeRange::AosTimeRange(const AosXmlTagPtr &request, const AosRundataPtr &rdata)
{
	aos_assert(request);
	AosTimeGran::E time_gran = AosTimeGran::toEnum(request->getAttrStr(AOSTAG_TIME_GRAN));
	if (!AosTimeGran::isValid(time_gran)) 
	{
		mStartTime.setInvalid();
		mEndTime.setInvalid();
		return;
	}

	OmnString startTime = request->getAttrStr(AOSTAG_COUNTERSTARTTIME, "");
	OmnString endTime = request->getAttrStr(AOSTAG_COUNTERENDTIME, "");
	OmnNotImplementedYet;
	//mStartTime.setTimeByStr(startTime);
	//mEndTime.setTimeByStr(endTime);
}

