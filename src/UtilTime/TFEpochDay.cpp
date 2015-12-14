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
// 08/12/2012 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TFEpochDay.h"

#include "UtilTime/TimeUtil.h"


AosTFEpochDay::AosTFEpochDay(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_EPOCH_DAY, eEpochDay, flag)
{
}


AosTFEpochDay::~AosTFEpochDay()
{
}


bool
AosTFEpochDay::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data' and
	// its converted value is saved in 'value'.
	aos_assert_r(data && len > 0, false);

	OmnString s(data, len);

	// Chen Ding, 2014/01/29
	// u64 day = s.toU64();
	i64 day;
	u32 nn;
	s.parseInt64(0, nn, day);

	// Chen Ding, 2014/01/29
	// if (day == 0)
	// {
	// 	value.setValue(day);
	// 	return true;
	// }
	i64 second = day * AOSTIMEFORMAT_SECONDS_PRE_DAY;

	switch (target_format)
	{
	case eYYMMDDHHMMSS:
		 {
			 value.setStr(AosTimeUtil::epochToStrNew("%y%m%d%H%M%S", second));
		 	 return true;
		 }

	case eYYYYMMDDHHMMSS:
		 {
			 value.setStr(AosTimeUtil::epochToStrNew("%Y%m%d%H%M%S", second));
		 	 return true;
		 }
	
	case eYYYY_MM_DDHHMMSS:
		 {
			 // Ketty 2014/11/04
			 value.setStr(AosTimeUtil::epochToStrNew("%Y-%m-%d %H:%M:%S", second));
		 	 return true;
		 }

	case eYYMMDD:
		 {
			 value.setStr(AosTimeUtil::epochToStrNew("%y%m%d", second));
		 	 return true;
		 }

	case eYYYYMMDD:
		 {
			 value.setStr(AosTimeUtil::epochToStrNew("%Y%m%d", second));
		 	 return true;
		 }
 
	case eEpochTime:
		 {
			 value.setI64(second);
			 return true;
		 }

	case eEpochDay:
		 {
			 value.setI64(day);
		  	 return true;
		 }
	
	case eEpochHour:
		 {
			 u64 hour = day * AOSTIMEFORMAT_HOURS_PRE_DAY;
			 value.setI64(hour);
		  	 return true;
		 }
	
	case eEpochWeek:
		 {
		 	// Ketty 2014/12/15
			value.setI64(AosTimeUtil::getEpochWeek(second));
		 	return true;
		 }

	case eEpochMonth:
		 {
			 value.setI64(AosTimeUtil::getEpochMonth(second));
		  	 return true;
		 }

	case eDayOfWeek:
		 {
			 value.setI64(AosTimeUtil::getDayOfWeek(second));
		  	 return true;
		 }
	
	case eDayOfMonth:
		 {
			 value.setI64(AosTimeUtil::getDayOfMonth(second));
		  	 return true;
		 }

	case eHourOfDay:
		 {
			 value.setI64(AosTimeUtil::getHourOfDay(second));
		  	 return true;
		 }

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_time_format") << enderr;
	return false;
}


/*
bool
AosTFEpochDay::check(const AosRundataPtr &rdata) const
{
	//u64 second = AosTimeUtil::mktime(2000, 0, 0, 0, 0, 0) + (60 * 60 * 24);

	static struct tm t1 = {0, 0, 8, 1, 0, 100};
	time_t second = mktime(&t1) + rand()%100;
	u64 day = second / (60 * 60 * 24);
	OmnString digitstr;
	digitstr << day;
	
	int format = rand() % (eMax - 1) + 1;
	AosValueRslt value;
	aos_assert_r(convert(digitstr.getBuffer(), digitstr.length(), (E)format, value, rdata), false);
	aos_assert_r(checkRslt((E)format, day, value, rdata), false);
	return true;
}


bool
AosTFEpochDay::checkRslt(
		const E &target_formart,
		const u64 &day,
		const AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	AosValueRslt vs;
	switch(target_formart)
	{
	case eYYMMDDHHMMSS:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYMMDDHHMMSS);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochDay, vs, rdata), false);
			u64 docid;
			aos_assert_r(vs.getU64Value(docid, rdata), false);
			aos_assert_r(docid == day, false);
		 	return true;
		}
	
	case eYYYYMMDDHHMMSS:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYYYMMDDHHMMSS);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochDay, vs, rdata), false);
			u64 docid;
			aos_assert_r(vs.getU64Value(docid, rdata), false);
			aos_assert_r(docid == day, false);
		 	return true;
		}
	
	case eYYMMDD:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYMMDD);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochDay, vs, rdata), false);
			u64 docid;
			aos_assert_r(vs.getU64Value(docid, rdata), false);
			aos_assert_r(docid == day, false);
		 	return true;
		}
	
	case eYYYYMMDD:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYYYMMDD);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochDay, vs, rdata), false);
			u64 docid;
			aos_assert_r(vs.getU64Value(docid, rdata), false);
			aos_assert_r(docid == day, false);
		 	return true;
		}

	case eEpochTime:
		 {
			 OmnString vv;
			 u64 second;
			 aos_assert_r(value.getU64Value(second, rdata), false);
			 vv << second;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochTime);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochDay, vs, rdata), false);
			 u64 docid;
			 aos_assert_r(vs.getU64Value(docid, rdata), false);
			 aos_assert_r(docid == day, false);
		 	 return true;
		 }

	case eEpochDay:
		 {
			 u64 docid;
			 aos_assert_r(value.getU64Value(docid, rdata), false);
			 aos_assert_r(docid == day, false);
			 return true;
		 }
	
	case eEpochHour:
		 {
			 OmnString vv;
			 u64 hour;
			 aos_assert_r(value.getU64Value(hour, rdata), false);
			 vv << hour;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochHour);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochDay, vs, rdata), false);
			 u64 docid;
			 aos_assert_r(vs.getU64Value(docid, rdata), false);
			 aos_assert_r(docid == day, false);
		  	 return true;
		 }

	default:
		break;
	}
	return false;
}
*/
