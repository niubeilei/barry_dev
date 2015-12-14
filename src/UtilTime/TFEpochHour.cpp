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
#include "UtilTime/TFEpochHour.h"

#include "UtilTime/TimeUtil.h"


AosTFEpochHour::AosTFEpochHour(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_EPOCH_HOUR, eEpochHour, flag)
{
}


AosTFEpochHour::~AosTFEpochHour()
{

}


bool
AosTFEpochHour::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data', and
	// its converted value is saved in 'value'.
	aos_assert_r(data && len > 0, false);

	OmnString s(data, len);

	// Chen Ding, 2014/01/29
	// u64 hour = s.toU64();
	i64 hour;
	u32 nn;
	s.parseInt64(0, nn, hour);
	// aos_assert_r(hour > 0, false);
	u64 second = hour * AOSTIMEFORMAT_SECONDS_PRE_HOUR;
	
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
			 u64 day = hour / AOSTIMEFORMAT_HOURS_PRE_DAY;
			 value.setI64(day);
			 return true;
		 }

	case eEpochHour:
		 {
			 value.setI64(hour);
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
AosTFEpochHour::check(const AosRundataPtr &rdata) const
{
	static struct tm t1 = {0, 0, 8, 1, 0, 100};
	time_t second = mktime(&t1) + rand();               
	u64 hh = second / (60 * 60);
	OmnString digitstr;
	digitstr << hh; 
	int format = rand() % (eMax - 1) + 1;
	AosValueRslt value;
	aos_assert_r(convert(digitstr.getBuffer(), digitstr.length(), (E)format, value, rdata), false);
	aos_assert_r(checkRslt((E)format, hh, value, rdata), false);
	return true;
}


bool
AosTFEpochHour::checkRslt(
		const E &target_format,
		const u64 &hour,
		const AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	AosValueRslt vs;
	switch(target_format)
	{
	case eYYMMDDHHMMSS:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYMMDDHHMMSS);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochHour, vs, rdata), false);
			u64 docid;
			aos_assert_r(vs.getU64Value(docid, rdata), false);
			aos_assert_r(docid == hour, false);
		 	return true;
		}
	
	case eYYYYMMDDHHMMSS:
		{
			return true;
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYYYMMDDHHMMSS);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochHour, vs, rdata), false);
			u64 docid;
			aos_assert_r(vs.getU64Value(docid, rdata), false);
			aos_assert_r(docid == hour, false);
		 	return true;
		}
	
	case eYYMMDD:
		{
		 	return true;
		}
	
	case eYYYYMMDD:
		{
		 	return true;
		}

	case eEpochTime:
		 {
			 OmnString vv;
			 u64 second;
			 aos_assert_r(value.getU64Value(second, rdata), false);
			 vv << second;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochTime);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochHour, vs, rdata), false);
			 u64 docid;
			 aos_assert_r(vs.getU64Value(docid, rdata), false);
			 aos_assert_r(docid == hour, false);
		 	 return true;
		 }

	case eEpochDay:
		 {
			 OmnString vv;
			 u64 hh;
			 aos_assert_r(value.getU64Value(hh, rdata), false);
			 vv << hh;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochDay);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eEpochHour, vs, rdata), false);
			 u64 docid;
			 aos_assert_r(vs.getU64Value(docid, rdata), false);
		 	 aos_assert_r(docid + hour%24 == hour, false);
		  	 return true;
		 }
	
	case eEpochHour:
		 {
			 u64 docid;
			 aos_assert_r(value.getU64Value(docid, rdata), false);
			 aos_assert_r(docid == hour, false);
		  	 return true;
		 }

	default:
			  break;
	}
	return false;
}
*/
