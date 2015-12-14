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
// 07/22/2013 Created by Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TFYYYYDOTMM.h"

#include "UtilTime/TimeUtil.h"


AosTFYYYYDOTMM::AosTFYYYYDOTMM(const bool flag)
:
AosTimeFormat(AOSTIMEFORMAT_YYYYDOTMM, eYYYYDOTMM, flag)
{
}


AosTFYYYYDOTMM::~AosTFYYYYDOTMM()
{
}


bool
AosTFYYYYDOTMM::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	// This function converts the time value from 'orig_format' to 
	// 'target_format'. The original value is stored in 'data', and
	// its converted value is saved in 'value'.
	aos_assert_r(data && len >= 7, false);

	switch (target_format)
	{
	case eYYYYMMDDHHMMSS:
		 {
			 OmnString ss(data, 8);
			 ss << "000000";
			 value.setStr(ss);
			 return true;
		 }

	case eYYYYMMDD:
		 {
			 OmnString ss(data, 8);
			 value.setStr(ss);
		 	 return true;
		 }

	case eYYMMDDHHMMSS:
		 {
			 OmnString ss(&data[2], 6);
			 ss << "000000";
			 value.setStr(ss);
		 	 return true;
		 }
	
	case eYYMMDD:
		 {
			 OmnString ss(&data[2], 6);
			 value.setStr(ss);
		 	 return true;
		 }

	case eEpochTime:
		 {
			 value.setI64(str2EpochTime(data, len));
			 return true;
		 }

	case eEpochDay:
		 {
		 	 value.setI64(str2EpochDay(data, len));
			 return true;
		 }

	case eEpochHour:
		 {
		 	 value.setI64(str2EpochHour(data, len));
			 return true;
		 }
	
	case eEpochMonth:
		 {
			 value.setI64(str2EpochMonth(data, len));
		  	 return true;
		 }
	
	case eDayOfWeek:
		 {
			 i64 second = str2EpochTime(data, len);
			 value.setI64(AosTimeUtil::getDayOfWeek(second));
		  	 return true;
		 }
	
	case eDayOfMonth:
		 {
			 i64 second = str2EpochTime(data, len);
			 value.setI64(AosTimeUtil::getDayOfMonth(second));
		  	 return true;
		 }

	case eHourOfDay:
		 {
		 	 i64 second = str2EpochTime(data, len);
			 value.setI64(AosTimeUtil::getHourOfDay(second));
		 	 return true;
		 }

	default:
		  break;
	}

	AosSetErrorU(rdata, "unrecog_time_format") << enderr;
	return false;
}


i64
AosTFYYYYDOTMM::str2EpochTime(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// 		yyyy.mm	
	// This function converts this string into a u32 Epoch Time representation
	// of the time.
	aos_assert_r(data && len >= 7, 0);
	
	OmnString yy = OmnString(data, 4);
	int year = yy.toInt();

	// Retrieve month
	OmnString mm = OmnString(&data[5], 2);
	int month = mm.toInt();
	if (month < 0 || month > 12) month = 0;
	
	// Retrieve day
	int day = 1; 
	int hour = 0;
	int min = 0;
	int sec = 0;
	
	return AosTimeUtil::mktimeNew(year, month, day, hour, min, sec);
}


int
AosTFYYYYDOTMM::str2EpochDay(const char *data, const int len) const
{
	// This function assumes 'timestr' is in the form:
	// 		yyyymmddhhmmss
	// This function converts this string into a u16 Epoch Day representation
	// of the time.
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_DAY;
}


int
AosTFYYYYDOTMM::str2EpochHour(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return second / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
}


int
AosTFYYYYDOTMM::str2EpochMonth(const char *data, const int len) const
{
	i64 second = str2EpochTime(data, len);
	return AosTimeUtil::getEpochMonth(second);
}


/*
bool
AosTFYYYYDOTMM::check(const AosRundataPtr &rdata) const
{
	static struct tm t1 = {0, 0, 8, 1, 0, 100};
	time_t second = mktime(&t1);               
	OmnString digitstr = AosTimeUtil::epochToStr("%Y%m%d", second + rand());

	int format = rand() % (eMax - 1) + 1;
	AosValueRslt value;
	aos_assert_r(convert(digitstr.getBuffer(), digitstr.length(), (E)format, value, rdata), false);
	aos_assert_r(checkRslt((E)format, digitstr, value, rdata), false);
	return true;
}


bool
AosTFYYYYDOTMM::checkRslt(
		const E &target_format,
		const OmnString &digitstr,
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
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			aos_assert_r(vs.getValueStr1("") == digitstr, false);
			return true;
		}

	case eYYYYMMDDHHMMSS:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYYYMMDDHHMMSS);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			aos_assert_r(vs.getValueStr1("") == digitstr, false);
			return true;
		}
	case eYYMMDD:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYMMDD);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			aos_assert_r(vs.getValueStr1("") == digitstr, false);
			return true;
		}
	
	case eYYYYMMDD:
		{
			OmnString vv = value.getValueStr1("");
			aos_assert_r(vv != "", false);
			AosTimeFormatPtr format = AosTimeFormat::getFormat(eYYYYMMDD);
			aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			aos_assert_r(vs.getValueStr1("") == digitstr, false);
			return true;
		}

	case eEpochTime:
		 {
			 OmnString vv;
			 u64 second;
			 aos_assert_r(value.getU64Value(second, rdata), false); 
			 vv << second;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochTime);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			 aos_assert_r(vs.getValueStr1("") == digitstr, false);
		 	 return true;
		 }

	case eEpochDay:
		 {
			 OmnString vv;
			 u64 day;
			 aos_assert_r(value.getU64Value(day, rdata), false);
			 vv << day;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochDay);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			 aos_assert_r(vs.getValueStr1("") == digitstr, false);
		 	 return true;
		}

	case eEpochHour:
		 {
			 OmnString vv;
			 u64 hour;
			 aos_assert_r(value.getU64Value(hour, rdata), false);
			 vv << hour;
			 AosTimeFormatPtr format = AosTimeFormat::getFormat(eEpochHour);
			 aos_assert_r(format->convert(vv.getBuffer(), vv.length(), eYYYYMMDD, vs, rdata), false);
			 aos_assert_r(vs.getValueStr1("") == digitstr, false);
		  	 return true;
		 }

	default:
			  break;
	}
	return false;
}
*/
