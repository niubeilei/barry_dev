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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TimeSelector/TimeMonthWeek2Epoch.h"

#include "Rundata/Rundata.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "ValueSel/ValueSel.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;


AosTimeMonthWeek2Epoch::AosTimeMonthWeek2Epoch(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_MONTHWEEK2EPOCH, AosTimeSelectorType::eMonthWeek2Epoch, reg)
{
}


AosTimeMonthWeek2Epoch::~AosTimeMonthWeek2Epoch()
{
}


bool  
AosTimeMonthWeek2Epoch::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//<sdoc zky_data_type="int64" zky_type="xxxxx" every="month">
	//	<day zky_data_type="int64" zky_value_type="const">1-31(28, 29, 30)</day>
	//	<hour zky_data_type="int64" zky_value_type="const">1-24</hour>
	//	<minute zky_data_type="int64" zky_value_type="const">0-59</minute>
	//	<second zky_data_type="int64" zky_value_type="const">0-59</second>
	//</sdoc>
	//
	//<sdoc zky_data_type="int64" every="week">
	//	<week zky_data_type="int64" zky_value_type="const">1-7</week>
	//	<hour zky_data_type="int64" zky_value_type="const">1-24</hour>
	//	<minute zky_data_type="int64" zky_value_type="const">0-59</minute>
	//	<second zky_data_type="int64" zky_value_type="const">0-59</second>
	//</sdoc>
	aos_assert_r(sdoc, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "data_type_incorrect") << datatype << enderr;
		return false;
	}
	//Current time
	date today = day_clock::local_day();
	OmnString every = sdoc->getAttrStr("every");
	aos_assert_r(every != "", false);
	//0000/00/00 00:00:00
	int64_t total_second = AosGetEpochTime(today.year(), today.month(), today.day(), 0, 0, 0);
	if (every == "month")
	{
		AosXmlTagPtr dayTag = sdoc->getFirstChild("day");
		aos_assert_r(dayTag, false);
		int day = 0;
		getValue(day, dayTag, rdata);
		int crtDay = today.day();
		int endDay = today.end_of_month().day();
		if (day > endDay)
		{
			day = endDay;
		}

		int delt = 0;
		if (crtDay <= day)
		{
			delt = day - crtDay;
		}
		else
		{
			delt = endDay - crtDay + day;
		}

		total_second += getDetailsTime(delt, sdoc, rdata);
	}
	else if (every == "week")
	{
		AosXmlTagPtr weekTag = sdoc->getFirstChild("week");
		aos_assert_r(weekTag, false);
		int week = 1;		
		getValue(week, weekTag, rdata);
		int crtWeek = today.day_of_week();
		if (crtWeek == 0)
		{
			crtWeek = 7;
		}
		int delt = 0;
		if (crtWeek > week)
		{
			delt = crtWeek - week;
		}
		else
		{
			delt = 7 - crtWeek + week;
		}
		total_second += getDetailsTime(delt, sdoc, rdata);
	}

	OmnString vv;
	vv << total_second;
	value.setStr(vv);
	return true;
}


bool  
AosTimeMonthWeek2Epoch::getValue(
		int &value,
		const AosXmlTagPtr &tag, 
		const AosRundataPtr &rdata)
{
	AosValueRslt valueRslt;
	if (!AosValueSel::getValueStatic(valueRslt, tag, rdata))
	{
		// Failed retrieving the value.
		AosSetErrorU(rdata, "time_failed_001") << tag->toString() << enderr;
		return false;
	}
	value = valueRslt.getI64();
	return true;
}


int64_t
AosTimeMonthWeek2Epoch::getDetailsTime(
		const int64_t &delt,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	int64_t total_second = delt * 24 * 60 * 60;
	AosXmlTagPtr hourTag = sdoc->getFirstChild("hour");
	if (hourTag)
	{
		int hour = 0;
		getValue(hour, hourTag, rdata);
		total_second += hour * 60 * 60;
	}
	AosXmlTagPtr minuteTag = sdoc->getFirstChild("minute");
	if (minuteTag)
	{
		int minute = 0;
		getValue(minute, minuteTag, rdata);
		total_second += minute * 60;
	}
	AosXmlTagPtr secondTag = sdoc->getFirstChild("second");
	if (secondTag)
	{
		int second = 0;
		getValue(second, secondTag, rdata);
		total_second += second;
	}
	return total_second;
}

