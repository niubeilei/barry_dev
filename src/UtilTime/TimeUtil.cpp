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
#include "UtilTime/TimeUtil.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Porting/GetTime.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"

/*
bool 	
AosTimeUtil::getTime(
		const OmnString &timestr, 
		const OmnString &format,
		AosTime &timeinfo)
{
	Format fmt = toEnum(format);
	switch (fmt)
	{
	case eYYYY:
		 // 'timestr' is a four digit string. 
		 aos_assert_r(timestr.length() == 4 && timestr.isDigitStr(), false);
		 timeinfo.setYear(atoi(timestr.data()));
		 aos_assert_r(timeinfo.getYear() >= 0, false);
		 return true;

	case eYYYYMM:
		 {
			 // 'timestr' is 'yyyy-mm'. 
			 OmnString mYear = timestr.substr(0,3);
			 OmnString mMonth;
			 if (timestr.data()[timestr.length()-2] == '-')
			 {
				 mMonth << "0" << timestr.data()[timestr.length()-1];
			 }
			 else
			 {
			 	 mMonth = timestr.substr(5,6);
			 }
			 aos_assert_r(mYear.length() == 4 && mYear.isDigitStr() && 
					 mMonth.length() == 2 && mMonth.isDigitStr(), false);
			 timeinfo.setYear(atoi(mYear.data()));
			 timeinfo.setMonth(atoi(mMonth.data()));
			 return true;
		 }
		 return true;

	case eYYYYMMDD:
		 {
			 // 'timestr' is 'yyyy-mm-dd'. 
			 const char *data = timestr.data();
			 OmnString mYear = timestr.substr(0,3);
			 const char *mMonth_ptr = strstr(data, "_");
			 aos_assert_r(mMonth_ptr, false);
			 const char *mDay_ptr = strstr(&data[mMonth_ptr - data], "_");
			 aos_assert_r(mDay_ptr, false);
			 timeinfo.setYear(atoi(mYear.data()));
			 timeinfo.setMonth(atoi(mMonth_ptr));
			 timeinfo.setDay(atoi(mDay_ptr));
			 aos_assert_r(timeinfo.getYear() > 0, false);
			 aos_assert_r(timeinfo.getMonth() >0 && timeinfo.getMonth() <= 12, false);
			 aos_assert_r(timeinfo.getDay() >= 1 && timeinfo.getDay() <= 31, false);
		 }
		 return true;
	
	default:
		 OmnAlarm << "Unrecognized time format: " << format << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 	
AosTimeUtil::getTime(AosTime &timeinfo)
{
	OmnString mYear, mMonth, mDay, mHour, mMinute;
	OmnGetTime(mYear, mMonth, mDay, mHour, mMinute);
	timeinfo.setYear(atoi(mYear.data())); 	
	timeinfo.setMonth(atoi(mMonth.data())); 	
	timeinfo.setDay(atoi(mDay.data())); 	
	timeinfo.setHour(atoi(mHour.data())); 	
	timeinfo.setMinute(atoi(mMinute.data()));

	return true;
}
*/

bool		
AosTimeUtil::toU32(
		const OmnString &timeValue,
		OmnString &newvalue,
		OmnString &errmsg)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTimeUtil::parseTimeNew(
		const AosXmlTagPtr &tag, 
		const OmnString &tagname,
		i64 &startime,
		i64 &endtime,
		AosTimeGran::E &time_gran,
		AosTime::TimeFormat	&format,
		bool use_epoch_time,
		OmnString &timeopr,
		const AosRundataPtr &rdata)
{
	startime = AosTime::eInvalidTime;
	endtime = AosTime::eInvalidTime;
	time_gran = AosTimeGran::eNoTime;
	aos_assert_rr(tag, rdata, false);
	aos_assert_rr(tagname != "", rdata, false);
	AosXmlTagPtr child = tag->getFirstChild(tagname);
	if (!child) return true;

	if (use_epoch_time)
	{
		return parseEpochTimeNew(child, startime, endtime, time_gran, format, timeopr, rdata);
	}

	return parseTimeNew(child, startime, endtime, time_gran, format, timeopr, rdata);
}


bool 	
AosTimeUtil::parseTimeNew(
		const AosXmlTagPtr &tag, 
		i64 &startime,
		i64 &endtime,
		AosTimeGran::E &time_gran,
		AosTime::TimeFormat	&format,
		OmnString &timeopr,
		const AosRundataPtr &rdata)
{
	// It assumes the following structure:
	// 	<tag AOSTAG_TIMEGRAN="AOSTIMEGRAN_WEEKLY/......." 
	// 		timeformat="xxx"
	// 		starttime="xxx" 
	// 		endtime="xxx"
	// 		zky_timenumber="1"/>
	// 	yyyy-mm-dd-hh-mm-ss	
	
	aos_assert_r(tag, false);
	OmnString stimeStr = tag->getAttrStr("zky_starttime", "");
	if (stimeStr == "") stimeStr = tag->getNodeText("zky_starttime");

	OmnString etimeStr = tag->getAttrStr("zky_endtime", "");
	if (etimeStr == "") etimeStr = tag->getNodeText("zky_endtime");

	OmnString timegran = tag->getAttrStr(AOSTAG_TIMEGRAN, "");
	if (timegran == "") timegran = tag->getNodeText(AOSTAG_TIMEGRAN);
	if (timegran == "") timegran = "ntm"; 

	timeopr = tag->getAttrStr("zky_timeopr", "and");	
	if (timeopr == "") timeopr = tag->getNodeText("zky_timeopr");

	time_gran = AosTimeGran::toEnum(timegran);
	aos_assert_r(AosTimeGran::isValid(time_gran), false);

	OmnString formatStr = tag->getAttrStr("zky_timeformat", "2fmt");
	format = AosTime::convertTimeFormatWithDft(formatStr);
	if (stimeStr == "" || etimeStr == "")
	{
		int num = tag->getAttrInt("zky_timenumber", -1);
		OmnString type = tag->getAttrStr("zky_timetype", "");
		if (type == "") type = tag->getNodeText("zky_timetype"); 
		if (type == "") return true;
		//y,m,w,d,H,M,S
		aos_assert_r(AosTime::calculateTime(type, num, 
					stimeStr, etimeStr, rdata), false);
	}
	//startime = AosTime::getUniTime(format, time_gran, stimeStr);
	//endtime = AosTime::getUniTime(format, time_gran, etimeStr);
	startime = AosTime::getUniTime(format, AosTimeGran::eSecondly, stimeStr, "%Y-%m-%d %H:%M:%S");
	endtime = AosTime::getUniTime(format, AosTimeGran::eSecondly, etimeStr, "%Y-%m-%d %H:%M:%S");
	return true;
}


// Chen Ding, 2014/01/29
// bool
// AosTimeUtil::parseEpochTime(
// 		const AosXmlTagPtr &tag,
// 		u64 &startime,
// 		u64 &endtime,
// 		AosTimeGran::E &time_gran,
// 		AosTime::TimeFormat &format,
// 		OmnString &timeopr,
// 		const AosRundataPtr &rdata)
bool
AosTimeUtil::parseEpochTimeNew(
		const AosXmlTagPtr &tag,
		i64 &startime,
		i64 &endtime,
		AosTimeGran::E &time_gran,
		AosTime::TimeFormat &format,
		OmnString &timeopr,
		const AosRundataPtr &rdata)
{
	OmnString stimeStr = tag->getAttrStr("zky_starttime", "");
	if (stimeStr == "") stimeStr = tag->getNodeText("zky_starttime");

	OmnString etimeStr = tag->getAttrStr("zky_endtime", "");
	if (etimeStr == "") etimeStr = tag->getNodeText("zky_endtime");

	OmnString timegran = tag->getAttrStr(AOSTAG_TIMEGRAN, "");
	if (timegran == "") timegran = tag->getNodeText(AOSTAG_TIMEGRAN);
	if (timegran == "") timegran = "ntm"; 

	timeopr = tag->getAttrStr("zky_timeopr", "and");	
	if (timeopr == "") timeopr = tag->getNodeText("zky_timeopr");

	time_gran = AosTimeGran::toEnum(timegran);
	aos_assert_r(AosTimeGran::isValid(time_gran), false);

	OmnString formatStr = tag->getAttrStr("zky_timeformat", "2fmt");
	format = AosTime::convertTimeFormatWithDft(formatStr);

	if (stimeStr == "" || etimeStr == "")
	{
		int num = tag->getAttrInt("zky_timenumber", -1);
		OmnString type = tag->getAttrStr("zky_timetype", "");
		if (type == "") type = tag->getNodeText("zky_timetype"); 
		if (type != "")
		{
			//y,m,w,d,H,M,S
			aos_assert_r(AosTime::calculateTime(type, num, 
						stimeStr, etimeStr, rdata), false);
		}
	}

	// startime = temporaryMethod(stimeStr);
	// endtime = temporaryMethod(etimeStr);
	startime = str2EpochTime(stimeStr);
	endtime = str2EpochTime(etimeStr);
	// if (startime == 0) startime = AosTime::eInvalidTime;
	// if (endtime == 0)  endtime = AosTime::eInvalidTime;

	if (time_gran == AosTimeGran::eNoTime)
	{
		startime =  AosTime::eInvalidTime;
		endtime =  AosTime::eInvalidTime;
	}
	else
	{
		aos_assert_r(startime !=  AosTime::eInvalidTime, false);
		aos_assert_r(endtime !=  AosTime::eInvalidTime, false);
	}
	return true;
}                                                                       


bool
AosTimeUtil::str2EpochTime(
		const AosRundataPtr &rdata, 
		const OmnString &timestr, 
		i64 &epoch_time)
{
	// This function checks whether the inputs are correct. 
	// It assumes the input is:
	// 		yyyy-mm-dd hh:mm:ss
	int len = timestr.length();
	if (len < 4)
	{
		AosSetErrorUser(rdata, "timeutil_time_too_short") << timestr << enderr;
		return false;
	}

	char *data = (char *)timestr.data();
	// 1. Scan the year
	int idx = 0;
	char c;
	int year;
	while (idx < 4)
	{
		c = data[idx++];
		if (c < '0' || c > '9')
		{
			AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
			return false;
		}
	}

	if (len == 4) 
	{
		// It has only "yyyy".
		year = atoi(data);
		epoch_time = mktimeNew(year, 1, 1, 0, 0, 0);
		return true;
	}

	// Handle the month
	if (len < 7)
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	c = data[4]; data[4] = 0; year = atoi(data); data[4] = c;
	if (data[idx++] != '-')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	if (data[idx] < '0' || data[idx] > '9' ||
	    data[idx+1] < '0' || data[idx+1] > '9')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}
	idx += 2;
	int month = 0;
	if (len == 7) 
	{
		month = atoi(&data[5]);
		if (month < 1 || month > 12)
		{
			AosSetErrorUser(rdata, "timeutil_invalid_month") << timestr << enderr;
			return false;
		}
		epoch_time = mktimeNew(year, month, 1, 0, 0, 0);
		return true;
	}

	// Handle the day
	if (len < 10)
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	// Ketty 2014/02/17
	//c = data[7]; data[7] = 0; month = atoi(data); data[7] = c;
	c = data[7]; data[7] = 0; month = atoi(&data[5]); data[7] = c;
	if (month < 0 || month > 12)
	{
		AosSetErrorUser(rdata, "timeutil_invalid_month") << timestr << enderr;
		return false;
	}

	if (data[idx++] != '-')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	if (data[idx] < '0' || data[idx] > '9' ||
	    data[idx+1] < '0' || data[idx+1] > '9')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}
	idx += 2;
	int day = 0;
	if (len == 10) 
	{
		day = atoi(&data[8]);
		if (day < 1 || day > 31)
		{
			AosSetErrorUser(rdata, "timeutil_invalid_day") << timestr << enderr;
			return false;
		}
		epoch_time = mktimeNew(year, month, day, 0, 0, 0);
		return true;
	}

	// Handle the hour: "yyyy-mm-dd hh"
	if (len < 13)
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	c = data[10]; data[10] = 0; day = atoi(data); data[10] = c;
	if (data[idx++] != ' ')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}
	if (day < 0 || day > 31)
	{
		AosSetErrorUser(rdata, "timeutil_invalid_day") << timestr << enderr;
		return false;
	}

	if (data[idx] < '0' || data[idx] > '9' ||
	    data[idx+1] < '0' || data[idx+1] > '9')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}
	idx += 2;
	int hour = 0;
	if (len == 13) 
	{
		hour = atoi(&data[12]);
		if (hour < 0 || hour > 23)
		{
			AosSetErrorUser(rdata, "timeutil_invalid_hour") << timestr << enderr;
			return false;
		}
		epoch_time = mktimeNew(year, month, day, hour, 0, 0);
		return true;
	}

	// Handle minute: "yyyy-mm-dd hh:mm"
	if (len < 16)
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	c = data[13]; data[13] = 0; hour = atoi(data); data[13] = c;
	if (hour < 0 || hour > 23)
	{
		AosSetErrorUser(rdata, "timeutil_invalid_hour") << timestr << enderr;
		return false;
	}

	if (data[idx++] != ':')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	idx += 2;
	int minute = 0;
	if (len == 16) 
	{
		minute = atoi(&data[15]);
		if (minute < 0 || minute > 59)
		{
			AosSetErrorUser(rdata, "timeutil_invalid_minute") << timestr << enderr;
			return false;
		}
		epoch_time = mktimeNew(year, month, day, hour, minute, 0);
		return true;
	}

	// Handle the second: yyyy-mm-dd hh:mm:ss
	if (len != 19)
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	c = data[16]; data[16] = 0; minute = atoi(data); data[16] = c;
	if (minute < 0 || minute > 59)
	{
		AosSetErrorUser(rdata, "timeutil_invalid_minute") << timestr << enderr;
		return false;
	}

	if (data[idx++] != ':')
	{
		AosSetErrorUser(rdata, "timeutil_not_valid_time_str") << timestr << enderr;
		return false;
	}

	idx += 2;
	int second = 0;
	second = atoi(&data[18]);
	if (second < 0 || second > 60)
	{
		AosSetErrorUser(rdata, "timeutil_invalid_second") << timestr << enderr;
		return false;
	}
	epoch_time = mktimeNew(year, month, day, hour, minute, second);
	return true;
}


// Chen Ding, 2014/01/29
// u64
// AosTimeUtil::temporaryMethod(const OmnString &timestr)
i64
AosTimeUtil::str2EpochTime(const OmnString &timestr)
{
	// This function assumes 'timestr' is in the form:
	//      yyyy-mm-dd hh:mm:ss
	//      0123456789012345678
	// If the input string is not in this format, one should
	// converted it into this format.
	
	if (timestr == "") return 0;
	aos_assert_r(timestr.length() >= 7, 0);
	char *data = (char *)timestr.data();
	OmnString errmsg;

	char c = data[4];
	data[4] = 0;
	int year = atoi(data);
	data[4] = c;

	c = data[7];
	data[7] = 0;
	// Month: [1, 12]
	int month = atoi(&data[5]);
	if (month < 0 || month > 12) month = 1;
	data[7] = c;

	int day = 1;
	if (timestr.length() >= 10)
	{
		c = data[10];
		data[10] = 0;
		// Day :[1, 31]
		day = atoi(&data[8]);
		if (day < 1 || day > 31) day = 1;
		data[10] = c;
	}

	int hour = 0;
	int min = 0;
	int sec = 0;

	// Chen Ding, 2013/12/31
	//      yyyy-mm-dd hh:mm:ss
	//      0123456789012345678
	if (timestr.length() >= 13)
	{
		char dd[3];
		dd[0] = data[11];
		dd[1] = data[12];
		dd[2] = 0;
		hour = atoi(dd);
		if (day < 0 || hour > 23) hour = 0;
	}

	if (timestr.length() >= 16)
	{
		char dd[3];
		dd[0] = data[14];
		dd[1] = data[15];
		dd[2] = 0;
		min = atoi(dd);
		if (min < 0 || min > 59) min = 0;
	}

	if (timestr.length() >= 19)
	{
		sec = atoi(&data[17]);
		if (sec < 0 || sec > 59) sec = 0;
	}

	i64 second = mktimeNew(year, month, day, hour, min, sec);
	return second;
}


i64  
AosTimeUtil::mktimeNew(
		const int year, 
		const int mon,
		const int day, 
		const int hour,
		const int min, 
		const int sec)
{
	// Ken Lee, 2013/07/30
//	if (year < 1970)
//	{
//		return 0;
//	}

	// The following code is taken from http://blog.csdn.net/axx1611/article/details/1792827
	int m = mon;
	int y = year;
	if (0 >= (m -= 2))
	{    
		m += 12; 
		y -= 1;
	}

	return ((((unsigned long) (y/4 - y/100 + y/400 + 367*m/12 + day) + y*365 - 719499)*24	+ hour)*60 + min)*60 + sec; 
}


u16  
AosTimeUtil::mktime(const int year, const int mon, const int day)
{
	// Ken Lee, 2013/07/30
	if (year < 1970)
	{
		return 0;
	}

	int m = mon;
	int y = year;
	if (0 >= (int) (m -= 2))
	{    
		m += 12; 
		y -= 1;
	}

	return ((unsigned long) (y/4 - y/100 + y/400 + 367*m/12 + day) + y*365 - 719499); 
}


OmnString 
AosTimeUtil::epochToStr(
		const OmnString &format, 
		const char *data, 
		const int &datalen)
{
	//Convert from epoch to String date
	char buf[80];	
	i64 epoch;
	aos_assert_r(AosStr2Int64(data, datalen, true, epoch), "");
	time_t tt = epoch;

	//Time is converted to the international standard time zone
	tm ts = *gmtime(&tt);
	strftime(buf, sizeof(buf), format.data(), &ts);
	OmnString str(buf);
	return str;
}


OmnString 
AosTimeUtil::epochToStrNew(const OmnString &format, const i64 &epoch)
{
	char buf[80];	
	time_t tt = epoch;

	//Time is converted to the international standard time zone
	tm ts = *gmtime(&tt); // Modified by Young. 2014/09/16
	strftime(buf, sizeof(buf), format.data(), &ts);
	OmnString str(buf);
	return str;
}


OmnString
AosTimeUtil::convertToLocalTime(
		const OmnString &format,
		const char *data,
		const int &datalen)
{
	//Convert from epoch to String date
	char buf[80];
	i64 epoch;
	bool rslt = AosStr2Int64(data, datalen, true, epoch);
	aos_assert_r(rslt, "");
	time_t tt = epoch;
	//Time is converted to the local time zone
	struct tm ts = {0};
	localtime_r(&tt, &ts);
	strftime(buf, sizeof(buf), format.data(), &ts);
	OmnString str(buf);
	return str;                                          
}

	
int
AosTimeUtil::getDayOfMonth(const i64 second)
{
	if (second == 0) return 0;
	struct tm t;
	gmtime_r((time_t *)&second, &t);
	return t.tm_mday;
}


int
AosTimeUtil::getHourOfDay(const i64 second)
{
	if (second == 0) return 0;
	struct tm t;
	gmtime_r((time_t *)&second, &t);
	return t.tm_hour;
}


int
AosTimeUtil::getDayOfWeek(const i64 second)
{
	if (second == 0) return 0;
	struct tm t;
	gmtime_r((time_t *)&second, &t);
	return t.tm_wday;
}


int
AosTimeUtil::getEpochMonth(const i64 second)
{
	if (second == 0) return 0;
	struct tm t;
	gmtime_r((time_t *)&second, &t);
	int month = (t.tm_year - 70) * 12 + (t.tm_mon);
	return month;
}


int
AosTimeUtil::getEpochYear(const i64 second)
{
	if (second == 0) return 0;
	struct tm t;
	gmtime_r((time_t *)&second, &t);
	return t.tm_year - 70;
}

int
AosTimeUtil::getEpochDay(const i64 second)
{
	return second / AOSTIMEFORMAT_SECONDS_PRE_DAY;
}

int
AosTimeUtil::getEpochHour(const i64 second)
{
	return second / AOSTIMEFORMAT_SECONDS_PRE_HOUR;
}


int
AosTimeUtil::getEpochWeek(const i64 second)
{
	/*
	 * 2015-07-24 Phil Get absolute week number
	 * without considering which day is the first
	 * day
	int epochday = AosTimeUtil::getEpochDay(second);
	if (epochday < 4) return 0;
	int week = 1 + (epochday - 3) / 7;
	return week;
	*/
	return second / AOSTIMEFORMAT_SECONDS_PRE_WEEK;
}

/*
int
AosTimeUtil::getEpochWeek(const i64 second)
{
	if (second == 0) return 0;
	struct tm t;
	gmtime_r((time_t *)&second, &t);

	//Week number of the year as a decimal number (01-53), 
	//where Monday is the first day of the week. 
	//The week containing January 1 is in the new year 
	//if four or more days are in the new year, 
	//otherwise it is the last week of the previous year
	char str[10];
	strftime(str, 10, "%V", &t);
	i64 w = atoi(str);
	i64 week = (t.tm_year - 70) * 53 + w;
	return week;
}*/


bool
AosTimeUtil::calculateTimeToPtime(
		const int number,
		const OmnString &timetype,
		ptime &crt_ptime,
		ptime &cal_ptime,
		const AosRundataPtr &rdata)
{
	i64 e1, e2;
	return calculateTimeNew(number, timetype, crt_ptime, cal_ptime, e1, e2, rdata);
}


bool
AosTimeUtil::calculateTimeNew(
		const int number,
		const OmnString &timetype,
		ptime &crt_ptime,
		ptime &cal_ptime,
		i64 &crt_epoch,
		i64 &cal_epoch,
		const AosRundataPtr &rdata)
{
	//y m d H M S w 
	//get Current time
	time_t lt = time(0);
	// coordinated universal time
	ptime p1 = from_time_t(lt); 
	if (p1.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format") << enderr;
		return false;
	}
	int num = number;
	if (num == 0) num = 1;

	date d = p1.date();
	if (d.is_not_a_date())
	{
		AosSetErrorU(rdata, "missing_data_format") << enderr;
		return false;
	}

	ptime p2;
	char c = timetype.data()[0];
	switch(c)
	{
		case 'H':
			 p2 = procHour(rdata, p1, num);
			 break;

		case 'M':
			 p2 = procMinute(rdata, p1, num);
			 break;

		case 'S':
			 p2 = procSecond(rdata, p1, num);
			 break;

		case 'y':
			 p2 = procYear(rdata, d, num);
			 break;

		case 'm':
			 p2 = procMonth(rdata, d, num);
			 break;

		case 'd':
			 p2 = procDay(rdata, d, num);
			 break;

		case 'w':
			 p2 = procWeek(rdata, d, num);
			 break;

		default:
			 AosSetErrorU(rdata, "invalid_from-now") << enderr;
		    return false;
	}

	if (p2.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_formatnow") << enderr;
		return false;
	}

	crt_ptime = p1;
	cal_ptime = p2;

	time_duration td = p1 - p2;
	int nanosec = td.total_seconds();
	cal_epoch = lt - nanosec; 
	crt_epoch = lt;
	/*
	time_duration td = p1 - p2;
	int nanosec = td.total_seconds();
	OmnString epoch;
	epoch << lt - nanosec; 

	OmnString crtepoch;
	int int_time = lt;
	crtepoch << int_time;

	if (crtepoch  == "" || epoch == "")
	{
		rdata->setError() << "Missing time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	crt_time = crtepoch;
	cal_time = epoch;
	*/
	return true;
}

ptime
AosTimeUtil::procHour(const AosRundataPtr &rdata, ptime p1, int num)
{
	ptime p2;
	if (p1.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return p2;
	}
	p2 = p1 + hours(num);
	return p2;
}

ptime
AosTimeUtil::procMinute(const AosRundataPtr &rdata, ptime p1, int num)
{
	ptime p2;
	if (p1.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return p2;
	}

	p2 = p1 + minutes(num);
	return p2;
}

ptime
AosTimeUtil::procSecond(const AosRundataPtr &rdata, ptime p1, int num)
{ 
	ptime p2;
	if (p1.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return p2;
	}
	p2 = p1 + seconds(num);
	return p2;
}

ptime
AosTimeUtil::procYear(const AosRundataPtr &rdata, date d, int num)
{
	ptime p2;
	if (d.is_not_a_date())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return p2;
	}
	if (num >0)
	{
		//for example: (num == 1)Current Time(2011-12-26 11:00:00) ------ 2011-12-31 23:59:59
		//for example: (num == 2)Current Time(2011-12-26 11:00:00) ------ 2012-12-31 23:59:59
		num = num -1;
		date dd(d.year(), 12, 31);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		dd = dd + years(num);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		ptime p3(dd, hours(23) + minutes(59) + seconds(59));
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	else
	{
		// for example: (num == -1)Current Time(2011-12-26 11:00:00) ---- 2011-01-01 00:00:00
		// for example: (num == -2)Current Time(2011-12-26 11:00:00) ---- 2011-01-01 00:00:00
		num = num + 1;
		date dd(d.year(), 1, 1);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		dd = dd + years(num);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		ptime p3(dd);
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3; 
	}
	return p2;
}


ptime
AosTimeUtil::procMonth(const AosRundataPtr &rdata, date d, int num)
{
	ptime p2;
	if (d.is_not_a_date())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return p2;
	}

	if (num > 0)
	{
		// for example: (num == 1) Current Time(2011-11-26 11:00:00) --- 2011-11-30 23:59:59
		// for example: (num == 2) Current Time(2011-11-26 11:00:00) --- 2011-12-31 23:59:59
		num = num -1;
		int day = AosTime::getDaysByYearAndMonth(d.year(), d.month());
		date dd(d.year(), d.month(), day);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		dd = dd + months(num);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		ptime p3(dd, hours(23) + minutes(59) + seconds(59));
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	else
	{
		// for example: (num == -1) Current Time(2011-12-26 11:00:00) --- 2011-12-01 00:00:00
		// for example: (num == -2) Current Time(2011-12-26 11:00:00) --- 2011-11-01 00:00:00
		num = num + 1;
		date dd(d.year(), d.month(), 1);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		dd = dd + months(num);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		ptime p3(dd);
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	return p2;
}


ptime
AosTimeUtil::procDay(const AosRundataPtr &rdata, date d, int num)
{
	ptime p2;
	if (d.is_not_a_date())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return p2;
	}

	if (num > 0)
	{
		//for example: (num  == 1)  Current Time(2011-12-26 11:00:00) 2011-12-26 23:59:59
		//for example: (num  == 2)  Current Time(2011-12-26 11:00:00) 2011-12-27 23:59:59
		num = num - 1;
		date dd(d.year(), d.month(), d.day());
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		dd = dd + days(num);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		ptime p3(dd, hours(23) + minutes(59) + seconds(59));
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	else
	{
		//for example: (num  == -1)  Current Time(2011-12-26 11:00:00) 2011-12-26 00:00:00
		//for example: (num  == -2)  Current Time(2011-12-26 11:00:00) 2011-12-25 00:00:00
		num = num + 1;
		date dd(d.year(), d.month(), d.day());
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			return p2;
		}
		dd = dd + days(num);
		if (dd.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_date_format");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return p2;
		}
		ptime p3(dd);
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	return p2;
}


ptime
AosTimeUtil::procWeek(const AosRundataPtr &rdata, date d, int num)
{
	ptime p2;
	if (d.is_not_a_date())
	{
		AosSetErrorU(rdata, "missing_data_format");
		return p2;
	}

	if (num > 0)
	{
		// for example: (num == 1)  Current Time(2011-12-26 11:00:00) --- 2011-12-31 23:59:59
		// for example: (num == 2) Current Time(2011-12-26 11:00:00) --- 2012-01-07 23:59:59
		num = num -1;
		int week = d.day_of_week();
		int value = 6 - week;
		d = d + days(value);
		if (d.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_data_format");
			return p2;
		}
		d = d + weeks(num);
		if (d.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_data_format");
			return p2;
		}
		ptime p3(d, hours(23) + minutes(59) + seconds(59));
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	else
	{
		//for example: (num  == -1)  Current Time(2011-12-26 11:00:00) 2011-12-25 00:00:00
		//for example: (num  == -2)  Current Time(2011-12-26 11:00:00) 2011-12-18 00:00:00
		num = num + 1;
		int week = d.day_of_week();
		d = d - days(week);
		if (d.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_data_format");
			return p2;
		}
		d = d + weeks(num);
		if (d.is_not_a_date())
		{
			AosSetErrorU(rdata, "missing_data_format");
			return p2;
		}
		ptime p3(d);
		if (p3.is_not_a_date_time())
		{
			AosSetErrorU(rdata, "missing_time_format");
			return p2;
		}
		p2 = p3;
	}
	return p2;
}

OmnString
AosTimeUtil::PtimeToLocalTime(const AosRundataPtr &rdata, ptime time)
{
	OmnString localtime;
	if (time.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return "";
	}
	time_zone_ptr zone(OmnNew posix_time_zone("GMT+8"));
	local_date_time az(time, zone);
	ostringstream os;
	time_facet *facet = OmnNew time_facet("%Y-%m-%d %H:%M:%S");
	os.imbue(locale(cout.getloc(), facet));
	os << az.local_time();
	localtime << os.str();
	return localtime; 
}

OmnString
AosTimeUtil::PtimeToStr(const AosRundataPtr &rdata, ptime time)
{
	OmnString localstr;
	if (time.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return "";
	}
	ostringstream os;
	time_facet *facet = OmnNew time_facet("%Y-%m-%d %H:%M:%S");
	os.imbue(locale(cout.getloc(), facet));
	os << time;
	localstr<< os.str();
	return localstr;
}


OmnString
AosTimeUtil::EpochDayToTimeNew(const AosRundataPtr &rdata, i64 epoch)
{
	OmnString localstr;
	time_t tt = epoch;
	ptime p = from_time_t(tt);
	if (p.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return "";
	}
	time_facet *facet = OmnNew time_facet("%Y-%m-%d");
	ostringstream os;
	os.imbue(locale(cout.getloc(), facet));
	os << p;
	localstr<< os.str();
	return localstr;
}

OmnString
AosTimeUtil::DayOfWeekToString(const AosRundataPtr &rdata, u64 value)
{
	 aos_assert_r(value>=0 && value <=6, "");
     OmnString s;
     switch(value)
     {
     case 1:
         s = "Mon";
         break;
     case 2:
         s = "Tues";
         break;
     case 3:
         s = "Wed";
         break;
     case 4:
         s = "Thur";
         break;
     case 5:
         s = "Fri";
         break;
     case 6:
         s = "Sat";
         break;
     case 0:
         s = "Sun";
         break;
     }
     return s;
}

OmnString
AosTimeUtil::EpochToTimeNew(i64 epoch, const OmnString &format)     
{
	OmnString localstr;
	time_t tt = epoch;
	ptime p = from_time_t(tt);
	if (p.is_not_a_date_time())
	{
		OmnAlarm << "missing time format!"<< enderr;
		return "";
	}
	time_facet *facet = OmnNew time_facet(format.data());
	ostringstream os;
	os.imbue(locale(cout.getloc(), facet));
	os << p;
	localstr<< os.str();
	return localstr;
}


OmnString
AosTimeUtil::EpochToLocalTimeNew(const AosRundataPtr &rdata, i64 epoch)
{
	OmnString localstr;
	time_t tt = epoch;
	struct tm tm2 = {0}; 
	localtime_r(&tt, &tm2);
	ptime p = ptime_from_tm(tm2);
	if (p.is_not_a_date_time())
	{
		AosSetErrorU(rdata, "missing_time_format");
		return "";
	}
	time_facet *facet = OmnNew time_facet("%Y-%m-%d %H:%M:%S");
	ostringstream os;
	os.imbue(locale(cout.getloc(), facet));
	os << p;
	localstr<< os.str();
	return localstr;
	/*
	OmnString localtime;
	ptime p = from_time_t(epoch);
	if (p.is_not_a_date_time())
	{
		rdata->setError() << "missing time format!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return "";
	}
	time_zone_ptr zone(OmnNew posix_time_zone("GMT+8"));
	local_date_time az(p, zone);
	ostringstream os;
	time_facet *facet = OmnNew time_facet("%Y-%m-%d %H:%M:%S");
	os.imbue(locale(cout.getloc(), facet));
	os << az.local_time();
	localtime << os.str();
	return localtime; 
	*/
}


int
AosTimeUtil::getNumDaysInMonth(const int year, const int month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		 return 31;

	case 4:
	case 6:
	case 9:
	case 11:
		 return 30;

	case 2:
		 if ((year % 4) == 0) return 29;
		 return 28;

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool 
AosTimeUtil::str2Time(
		const AosRundataPtr &rdata, 
		const OmnString &format,
		const OmnString &str_value,
		i64 &value)
{
	if (format == AOSTIMEFORMAT_EPOCH_DAY)
	{
		value = atoi(str_value.data());
		return true;
	}

	if (format == AOSTIMEFORMAT_YYYY_MM_DDHHMMSS)
	{
		int vv;
		bool rslt = str2EpochDay(rdata, str_value, vv);
		value = vv;
		return rslt;
	}

	AosSetErrorUser(rdata, "termiiltype_unrecog_format") 
		<< "Format: " << format << enderr;
	return false;
}


// Chen Ding, 2013/02/23
// Following is copied from http://www.oschina.net/code/snippet_948435_18557. 
// It converts any day to a weekday.
#if 0
#include "stdafx.h"
#include<iostream>
#include<string>
#include<vector>
//#include<algorithm>
using namespace std;

int a[]={2013,2,17};
vector<int> vec(a,a+3);
class time
{
public:
	time(){}
	time(const vector<int>& v):vec(v){}
	friend istream& operator>>(istream&,time&);
	friend ostream& operator<<(ostream&,const time&);
	friend long operator-(time&,time&);//¼ÆËãÁ½¸öÈÕÆÚµÄÏà¸ôÌìÊý
	bool test();//²âÊÔÊäÈëµÄÈÕÆÚÊÇ·ñºÏ·¨
	time* preday();//·µ»Ø¸ø¶¨ÈÕÆÚµÄÇ°Ò»Ìì
	bool compare(const time&);//Óë2013-2-17±È½Ï´óÐ¡
private:
	vector<int> vec;
};
long operator-(time& t1,time&t2)////Á½¸ö¾ßÌåÈÕÆÚµÄ¼õ·¨
{
	long sum=0;
	time tt1=t1,tt2=t2;/////tt1´æ·ÅµÄÈÕÆÚ´óÓÚtt2´æ·ÅµÄÈÕÆÚ
	if(!t1.compare(t2))//ÅÐ¶ÏÁ½¸öÈÕÆÚµÄ´óÐ¡
	{
		tt1=t2;
		tt2=t1;
	}
	if(tt1.vec[0]>tt2.vec[0])//ÅÐ¶ÏÁ½¸öÄê·Ý´óÐ¡£¬´óÓÚÔò·Ö±ð¼ÆËã¸ÃÁ½¸öÄê·ÝÖÐµÄÒªÇóÌìÊýºÍÁ½Äê·ÝÖ®¼äµÄÄê·ÝµÄÌìÊý
  {
	switch(tt2.vec[1])
	{
	case 1:
		sum+=365-tt2.vec[2];
		break;
	case 2:
		sum+=365-31-tt2.vec[2];
		break;
	case 3:
		sum+=365-31-28-tt2.vec[2];
		break;
	case 4:
		sum+=365-31-28-31-tt2.vec[2];
		break;
	case 5:
		sum+=365-31-28-31-30-tt2.vec[2];
		break;
	case 6:
		sum+=365-31-28-31-30-31-tt2.vec[2];
		break;
	case 7:
		sum+=365-31-28-31-30-31-30-tt2.vec[2];
		break;
	case 8:
		sum+=365-31-28-31-30-31-30-31-tt2.vec[2];
		break;
	case 9:
		sum+=365-31-28-31-30-31-30-31-31-tt2.vec[2];
		break;
	case 10:
		sum+=365-31-28-31-30-31-30-31-31-30-tt2.vec[2];
		break;
	case 11:
		sum+=365-31-28-31-30-31-30-31-31-30-31-tt2.vec[2];
		break;
	case 12:
		sum+=365-31-28-31-30-31-30-31-31-30-31-30-tt2.vec[2];
		break;
	}
	if((tt2.vec[0]%400==0||(tt2.vec[0]%100!=0&&tt2.vec[0]%4==0))&&(tt2.vec[1]==1||tt2.vec[1]==2))
		sum+=1;//ÈòÄê²¢Âú×ã(tt2.vec[1]==1||tt2.vec[1]==2)Ôò×ÜÌìÊý¼ÓÒ»
		for(long i=1;i<tt1.vec[0]-tt2.vec[0];i++)
			{
				if((tt2.vec[0]+i)%400==0||((tt2.vec[0]+i)%100!=0&&(tt2.vec[0]+i)%4==0))
					sum+=366;
				else
					sum+=365;
			}
	 switch(tt1.vec[1])//¼ÆËã´óµÄÈÕÆÚÖÐÒÑ¹ýµÄÌìÊý
	    {
	case 1:
		sum+=tt1.vec[2];
		break;
	case 2:
		sum+=31+tt1.vec[2];
		break;
	case 3:
		sum+=31+28+tt1.vec[2];
		break;
	case 4:
		sum+=31+28+31+tt1.vec[2];
		break;
	case 5:
		sum+=31+28+31+30+tt1.vec[2];
		break;
	case 6:
		sum+=31+28+31+30+31+tt1.vec[2];
		break;
	case 7:
		sum+=31+28+31+30+31+30+tt1.vec[2];
		break;
	case 8:
		sum+=31+28+31+30+31+30+31+tt1.vec[2];
		break;
	case 9:
		sum+=31+28+31+30+31+30+31+31+tt1.vec[2];
		break;
	case 10:
		sum+=31+28+31+30+31+30+31+31+30+tt1.vec[2];
		break;
	case 11:
		sum+=31+28+31+30+31+30+31+31+30+31+tt1.vec[2];
		break;
	case 12:
		sum+=31+28+31+30+31+30+31+31+30+31+30+tt1.vec[2];
		break;
	  }
	 if((tt1.vec[0]%400==0||(tt1.vec[0]%100!=0&&tt1.vec[0]%4==0))&&(tt1.vec[1]!=1&&tt1.vec[1]!=2))
		sum+=1;
  }
	else//Á½¸öÈÕÆÚµÄÄê·Ý¼´vec[0]ÏàµÈÊ±
	{
		if(tt2.vec[1]==tt1.vec[1])
			sum+=tt1.vec[2]-tt2.vec[2];
		else
		{
		switch(tt2.vec[1])
		  {
		case 1:
			switch(tt1.vec[1])
			{
			case 2:
				sum+=31-tt2.vec[2]+tt1.vec[2];
				break;
			case 3:
				sum+=31-tt2.vec[2]+28+tt1.vec[2];
				break;
			case 4:
				sum+=31-tt2.vec[2]+28+31+tt1.vec[2];
				break;
			case 5:
				sum+=31-tt2.vec[2]+28+31+30+tt1.vec[2];
				break;
			case 6:
				sum+=31-tt2.vec[2]+28+31+30+31+tt1.vec[2];
				break;
			case 7:
				sum+=31-tt2.vec[2]+28+31+30+31+30+tt1.vec[2];
				break;
			case 8:
				sum+=31-tt2.vec[2]+28+31+30+31+30+31+tt1.vec[2];
				break;
			case 9:
				sum+=31-tt2.vec[2]+28+31+30+31+30+31+31+tt1.vec[2];
				break;
			case 10:
				sum+=31-tt2.vec[2]+28+31+30+31+30+31+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=31-tt2.vec[2]+28+31+30+31+30+31+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=31-tt2.vec[2]+28+31+30+31+30+31+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 2:
			switch(tt1.vec[1])
			{
			case 3:
				sum+=28-tt2.vec[2]+tt1.vec[2];
				break;
			case 4:
				sum+=28-tt2.vec[2]+31+tt1.vec[2];
				break;
			case 5:
				sum+=28-tt2.vec[2]+31+30+tt1.vec[2];
				break;
			case 6:
				sum+=28-tt2.vec[2]+31+30+31+tt1.vec[2];
				break;
			case 7:
				sum+=28-tt2.vec[2]+31+30+31+30+tt1.vec[2];
				break;
			case 8:
				sum+=28-tt2.vec[2]+31+30+31+30+31+tt1.vec[2];
				break;
			case 9:
				sum+=28-tt2.vec[2]+31+30+31+30+31+31+tt1.vec[2];
				break;
			case 10:
				sum+=28-tt2.vec[2]+31+30+31+30+31+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=28-tt2.vec[2]+31+30+31+30+31+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=28-tt2.vec[2]+31+30+31+30+31+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 3:
			switch(tt1.vec[1])
			{
			case 4:
				sum+=31-tt2.vec[2]+tt1.vec[2];
				break;
			case 5:
				sum+=31-tt2.vec[2]+30+tt1.vec[2];
				break;
			case 6:
				sum+=31-tt2.vec[2]+30+31+tt1.vec[2];
				break;
			case 7:
				sum+=31-tt2.vec[2]+30+31+30+tt1.vec[2];
				break;
			case 8:
				sum+=31-tt2.vec[2]+30+31+30+31+tt1.vec[2];
				break;
			case 9:
				sum+=31-tt2.vec[2]+30+31+30+31+31+tt1.vec[2];
				break;
			case 10:
				sum+=31-tt2.vec[2]+30+31+30+31+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=31-tt2.vec[2]+30+31+30+31+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=31-tt2.vec[2]+30+31+30+31+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 4:
			switch(tt1.vec[1])
			{
			case 5:
				sum+=30-tt2.vec[2]+tt1.vec[2];
				break;
			case 6:
				sum+=30-tt2.vec[2]+31+tt1.vec[2];
				break;
			case 7:
				sum+=30-tt2.vec[2]+31+30+tt1.vec[2];
				break;
			case 8:
				sum+=30-tt2.vec[2]+31+30+31+tt1.vec[2];
				break;
			case 9:
				sum+=30-tt2.vec[2]+31+30+31+31+tt1.vec[2];
				break;
			case 10:
				sum+=30-tt2.vec[2]+31+30+31+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=30-tt2.vec[2]+31+30+31+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=30-tt2.vec[2]+31+30+31+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 5:
			switch(tt1.vec[1])
			{
			case 6:
				sum+=31-tt2.vec[2]+tt1.vec[2];
				break;
			case 7:
				sum+=31-tt2.vec[2]+30+tt1.vec[2];
				break;
			case 8:
				sum+=31-tt2.vec[2]+30+31+tt1.vec[2];
				break;
			case 9:
				sum+=31-tt2.vec[2]+30+31+31+tt1.vec[2];
				break;
			case 10:
				sum+=31-tt2.vec[2]+30+31+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=31-tt2.vec[2]+30+31+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=31-tt2.vec[2]+30+31+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 6:
			switch(tt1.vec[1])
			{
			case 7:
				sum+=30-tt2.vec[2]+tt1.vec[2];
				break;
			case 8:
				sum+=30-tt2.vec[2]+31+tt1.vec[2];
				break;
			case 9:
				sum+=30-tt2.vec[2]+31+31+tt1.vec[2];
				break;
			case 10:
				sum+=30-tt2.vec[2]+31+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=30-tt2.vec[2]+31+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=30-tt2.vec[2]+31+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 7:
			switch(tt1.vec[1])
			{
			case 8:
				sum+=31-tt2.vec[2]+tt1.vec[2];
				break;
			case 9:
				sum+=31-tt2.vec[2]+31+tt1.vec[2];
				break;
			case 10:
				sum+=31-tt2.vec[2]+31+30+tt1.vec[2];
				break;
			case 11:
				sum+=31-tt2.vec[2]+31+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=31-tt2.vec[2]+31+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 8:
			switch(tt1.vec[1])
			{
			case 9:
				sum+=31-tt2.vec[2]+tt1.vec[2];
				break;
			case 10:
				sum+=31-tt2.vec[2]+30+tt1.vec[2];
				break;
			case 11:
				sum+=31-tt2.vec[2]+30+31+tt1.vec[2];
				break;
			case 12:
				sum+=31-tt2.vec[2]+30+31+30+tt1.vec[2];
				break;
			}
			break;
		case 9:
			switch(tt1.vec[1])
			{
			case 10:
				sum+=30-tt2.vec[2]+tt1.vec[2];
				break;
			case 11:
				sum+=30-tt2.vec[2]+31+tt1.vec[2];
				break;
			case 12:
				sum+=30-tt2.vec[2]+31+30+tt1.vec[2];
				break;
			}
			break;
		case 10:
			switch(tt1.vec[1])
			{
			case 11:
				sum+=31-tt2.vec[2]+tt1.vec[2];
				break;
			case 12:
				sum+=31-tt2.vec[2]+30+tt1.vec[2];
				break;
			}
			break;
		case 11:
				sum+=30-tt2.vec[2]+tt1.vec[2];
				break;
		  }
		  if(tt1.vec[0]%400==0||(tt1.vec[0]%100!=0&&tt1.vec[0]%4==0))
		  {
			  if((tt2.vec[1]==1&&tt1.vec[1]!=2)||tt2.vec[1]==2)
				  sum+=1;
		  }
		}
	}
	if(t1.compare(t2))
	   return sum;
	return -sum;
}
bool time::compare(const time& tt)////´óÓÚÐÎ²ÎttÔò·µ»Øtrue
{
	if((vec[0]>tt.vec[0])||(vec[0]==tt.vec[0]&&vec[1]>tt.vec[1])||(vec[0]==tt.vec[0]&&vec[1]==tt.vec[1]&&vec[2]>tt.vec[2]))
		return true;
	return false;
}
istream& operator>>(istream& is,time& t)
{
	string st;
	while(getline(cin,st,'-'))
		t.vec.push_back(stoi(st));
	cin.clear();
	return is;
}
ostream& operator<<(ostream& os,const time& t)
{
	os<<t.vec[0]<<"-"<<t.vec[1]<<"-"<<t.vec[2];
	return os;
}
time* time::preday()
{
	vector<int> vvv;
	if((vec[1]==5||vec[1]==7||vec[1]==10||vec[1]==12)&&(vec[2]==1))
	{
		    vvv.push_back(vec[0]);
			vvv.push_back(vec[1]-1);
			vvv.push_back(30);
			return new time(vvv);
	}
	if((vec[1]==2||vec[1]==4||vec[1]==6||vec[1]==8||vec[1]==9||vec[1]==11)&&(vec[2]==1))
	{
		    vvv.push_back(vec[0]);
			vvv.push_back(vec[1]-1);
			vvv.push_back(31);
			return new time(vvv);
	}
	if(vec[1]==1&&vec[2]==1)
	{
		    vvv.push_back(vec[0]-1);
	        vvv.push_back(12);
			vvv.push_back(31);
			return new time(vvv);
	}
	if(vec[1]==3&&vec[2]==1)
	{
		    vvv.push_back(vec[0]);
	        vvv.push_back(2);
			if(vec[0]%400==0||(vec[0]%100!=0&&vec[0]%4==0))
			vvv.push_back(29);
			else
				vvv.push_back(28);
			return new time(vvv);
	}
	        vvv.push_back(vec[0]);
	        vvv.push_back(vec[1]);
			vvv.push_back(vec[2]-1);
			return new time(vvv);
}
bool time::test()
{
	int da[]={1,3,5,7,8,10,12};
	int xiao[]={4,6,9,11};
		if(find(da,da+7,vec[1])!=da+7)
		{
			
			if(vec[2]<1||vec[2]>31)
			{
			vec.clear();
			return false;
			}
			return true;
		}
	    if(vec[1]==2)
		{
			if(vec[0]%400==0||(vec[0]%100!=0&&vec[0]%4==0))
			{
			  if(vec[2]<1||vec[2]>29)
		       {
				vec.clear();
			return false;
			   }
				return true;
			}
			if(vec[2]<1||vec[2]>28)
		       {
				vec.clear();
			return false; 
			   }
			return true;
		}
		if(find(xiao,xiao+4,vec[1])!=xiao+4)
		{
			if(vec[2]<1||vec[2]>30)
		       {
				vec.clear();
			return false; 
			   }
			return true;
		}
		return false;
}
class daily
{
public:
	daily(const time& tt):daytime(new time(tt)),use(new int(1)){}
	daily(const daily&);
	daily& operator=(const daily&);
	void del(){if(--*use==0) delete daytime,use;}
	~daily(){del();}
	//int calculate();
	void display();
private:
	time *daytime;
	int *use;
	static time standardtime;
};
time daily::standardtime(vec);
void daily::display()
{
	long sum=*daytime-standardtime;
	if(sum>=0)
	{
		switch(sum%7)
		{
		case 0:
			cout<<"µ±ÌìÎªÐÇÆÚÈÕ"<<endl;
			break;
		case 1:
			cout<<"µ±ÌìÎªÐÇÆÚÒ»"<<endl;
			break;
		case 2:
			cout<<"µ±ÌìÎªÐÇÆÚ¶þ"<<endl;
			break;
		case 3:
			cout<<"µ±ÌìÎªÐÇÆÚÈý"<<endl;
			break;
		case 4:
			cout<<"µ±ÌìÎªÐÇÆÚËÄ"<<endl;
			break;
		case 5:
			cout<<"µ±ÌìÎªÐÇÆÚÎå"<<endl;
			break;
		case 6:
			cout<<"µ±ÌìÎªÐÇÆÚÁù"<<endl;
			break;
		}
	}
	else
	{
		switch((-sum)%7)
		{
		case 0:
			cout<<"µ±ÌìÎªÐÇÆÚÈÕ"<<endl;
			break;
		case 1:
			cout<<"µ±ÌìÎªÐÇÆÚÁù"<<endl;
			break;
		case 2:
			cout<<"µ±ÌìÎªÐÇÆÚÎå"<<endl;
			break;
		case 3:
			cout<<"µ±ÌìÎªÐÇÆÚËÄ"<<endl;
			break;
		case 4:
			cout<<"µ±ÌìÎªÐÇÆÚÈý"<<endl;
			break;
		case 5:
			cout<<"µ±ÌìÎªÐÇÆÚ¶þ"<<endl;
			break;
		case 6:
			cout<<"µ±ÌìÎªÐÇÆÚÒ»"<<endl;
			break;
		}
	}
}
daily::daily(const daily& d)
{
	++(*d.use);
	use=d.use;
	daytime=d.daytime;
}
daily& daily::operator=(const daily& d)
{
	++(*d.use);
	del();
	daytime=d.daytime;
	use=d.use;
	return *this;
}
int _tmain(int argc, _TCHAR* argv[])
{
	cout<<"ÈÕÆÚ²éÑ¯½çÃæ"<<endl<<endl;
	cout<<"ÇëÊäÈë¾ßÌåÈÕÆÚ(¸ñÊ½Îª00-00-00£©"<<endl;
	time t;
	cin>>t;
	if(!t.test())
	{
		cout<<"ÊäÈëµÄÈÕÆÚÓÐÎó£¡"<<endl;
		return 0;
	}
	daily d(t);
	d.display();
	return 0;
}
#endif
