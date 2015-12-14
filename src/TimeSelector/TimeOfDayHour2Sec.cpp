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
#include "TimeSelector/TimeOfDayHour2Sec.h"

#include "Rundata/Rundata.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;


AosTimeOfDayHour2Sec::AosTimeOfDayHour2Sec(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_OFDAYHOUR2SEC, AosTimeSelectorType::eOfDayHour2Sec, reg)
{
}


AosTimeOfDayHour2Sec::~AosTimeOfDayHour2Sec()
{
}


bool  
AosTimeOfDayHour2Sec::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	//<sdoc zky_data_type="" zky_number = "xxx" zky_type="hour2sec">
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "data_type_incorrect") << datatype << enderr;
	}
	//Current time
	time_t lt = time(0);
	ptime p1 = from_time_t(lt);
	if (p1.is_not_a_date_time())
	{
		AosSetErrorUser(rdata, "missing_time_format") << enderr;
		return false;
	}

	//2011/03/20 15:11:00 To Ptime 
	OmnString str;
	int number = sdoc->getAttrInt("zky_number", 0);
	number = number % 24;
	str << p1.date().year() << "/" << p1.date().month() << "/" << p1.date().day() 
	<< " " << number << ":00:00";
	aos_assert_r(str != "", false);

	//Format: yyyy/mm/dd HH:mm:ss 2011/03/20 15:11:00
	ptime p2 = time_from_string(str.data());
	if (p2.is_not_a_date_time())
	{
		AosSetErrorUser(rdata, "missing_time_format") << enderr;
		return false;
	} 
	
	//ptime p3 = p2 - hours(8);
	int  minute = OmnGetTimeZoneOffset();
	ptime p3 = p2 + minutes(minute); 
	if (p3.is_not_a_date_time())
	{
		AosSetErrorUser(rdata, "missing_time_format") << enderr;
	    return false;
	}

	time_duration td1 = p3 - p1;
	int nanosec = td1.total_seconds();
	if (nanosec <= 0)
	{
		// + 1 day;
		nanosec += 24 * 60 * 60;
	}

	OmnString vv;
	vv << (lt + nanosec);
	value.setStr(vv);
	return true;
}


