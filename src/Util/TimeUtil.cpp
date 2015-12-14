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
/*
#include "Util/TimeUtil.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Porting/GetTime.h"

bool 	
AosTimeUtil::getTime(
		const OmnString &timestr, 
		const OmnString &format,
		AosTimeInfo &timeinfo)
{
	Format fmt = toEnum(format);
	switch (fmt)
	{
	case eYYYY:
		 // 'timestr' is a four digit string. 
		 aos_assert_r(timestr.length() == 4 && timestr.isDigitStr(), false);
		 timeinfo.year = atoi(timestr.data());
		 aos_assert_r(timeinfo.year >= 0, false);
		 return true;

	case eYYYYMM:
		 {
			 // 'timestr' is 'yyyy-mm'. 
			 OmnString year = timestr.substr(0,3);
			 OmnString month;
			 if (timestr[timestr.length()-2] == '-')
			 {
				 month << "0" << timestr[timestr.length()-1];
			 }
			 else
			 	month = timestr.substr(5,6);
			 aos_assert_r(year.length() == 4 && year.isDigitStr() && 
					 month.length() == 2 && month.isDigitStr(), false);
			 timeinfo.year = atoi(year.data());
			 timeinfo.month = atoi(month.data());
			 return true;
		 }
		 break;
	
	default:
		break;
	}
	OmnShouldNeverComeHere;
	return false;
}

bool 	
AosTimeUtil::getTime(
		AosTimeInfo &timeinfo)
{
	OmnString year, month, day, hour, minute;
	OmnGetTime(year, month, day, hour, minute);
	timeinfo.year = atoi(year); 	
	timeinfo.month = atoi(month); 	
	timeinfo.day = atoi(day); 	
	timeinfo.hour = atoi(hour); 	
	timeinfo.minute = atoi(minute);

	return true;
}


AosTimeUtil::Format
AosTimeUtil::toEnum(const OmnString &format)
{
	if(format == AOSTIMEFORMAT_YYYY)	return eYYYY;
	if(format == AOSTIMEFORMAT_YYYYMM)	return eYYYYMM;

	return eInvalid;
}


bool 	
AosTimeUtil::toU32(const OmnString &timestr, OmnString &value, OmnString &errmsg)
{
	OmnNotImplementedYet;
	return false;
}
*/

