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
#ifndef Omn_UtilTime_TimeUtil_h
#define Omn_UtilTime_TimeUtil_h

#include "aosUtil/Types.h"
#include "Util/String.h"
#include "UtilTime/TimeInfo.h"

//#define AOSTIMEFORMAT_YYYY		"yyyy"
//#define AOSTIMEFORMAT_YYYYMM		"yyyy-mm"
//#define AOSTIMEFORMAT_YYYYMMDD	"yyyy-mm-dd"

class AosTime;

class AosTimeUtil : virtual public OmnRCObject
{
	OmnDefineRCObject;
public:
	enum
	{
		eSecondsPerDay = 60*60*24,
		eMinDay = -2147483647,
		eMaxDay = 2147483647
	};

public:
	AosTimeUtil();
	~AosTimeUtil();

 	static bool	getTime(
					const OmnString &timestr, 
					const OmnString &format,
					AosTime &timeinfo);

	static bool getTime(AosTime &timeinfo);

	static bool	toU32(
					const OmnString &timeValue,
					OmnString &newvalue,
					OmnString &errmsg);

	// Chen Ding, 2014/01/29
	// static bool parseTime(
	// 				const AosXmlTagPtr &tag, 
	// 				const OmnString &tagname,
	// 				u64 &startime,
	// 				u64 &endtime,
	// 				AosTimeGran::E &time_gran,
	// 				AosTime::TimeFormat	&format,
	// 				bool use_epoch_time,
	// 				const AosRundataPtr &rdata)
	static bool parseTimeNew(
					const AosXmlTagPtr &tag, 
					const OmnString &tagname,
					i64 &startime,
					i64 &endtime,
					AosTimeGran::E &time_gran,
					AosTime::TimeFormat	&format,
					bool use_epoch_time,
					const AosRundataPtr &rdata)
	{
		OmnString timeopr;
		return parseTimeNew(tag, tagname, startime, endtime, time_gran, 
			format, use_epoch_time, timeopr, rdata);
	}

	// Chen Ding, 2014/01/29
	// static bool parseTime(
	// 				const AosXmlTagPtr &tag, 
	// 				const OmnString &tagname,
	// 				u64 &startime,
	// 				u64 &endtime,
	// 				AosTimeGran::E &time_gran,
	// 				AosTime::TimeFormat	&format,
	// 				bool use_epoch_time,
	// 				OmnString &timeopr,
	// 				const AosRundataPtr &rdata);
	static bool parseTimeNew(
					const AosXmlTagPtr &tag, 
					const OmnString &tagname,
					i64 &startime,
					i64 &endtime,
					AosTimeGran::E &time_gran,
					AosTime::TimeFormat	&format,
					bool use_epoch_time,
					OmnString &timeopr,
					const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/29
	// static bool	parseTime(
	// 				const AosXmlTagPtr &tag, 
	// 				u64 &startime,
	// 				u64 &endtime,
	// 				AosTimeGran::E &time_gran,
	// 				AosTime::TimeFormat &format,
	// 				OmnString &timeopr,
	// 				const AosRundataPtr &rdata);
	static bool	parseTimeNew(
					const AosXmlTagPtr &tag, 
					i64 &startime,
					i64 &endtime,
					AosTimeGran::E &time_gran,
					AosTime::TimeFormat &format,
					OmnString &timeopr,
					const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/29
	// static bool	parseEpochTime(
	// 				const AosXmlTagPtr &tag, 
	// 				u64 &startime,
	// 				u64 &endtime,
	// 				AosTimeGran::E &time_gran,
	// 				AosTime::TimeFormat &format,
	// 				OmnString &timeopr,
	// 				const AosRundataPtr &rdata);
	static bool	parseEpochTimeNew(
					const AosXmlTagPtr &tag, 
					i64 &startime,
					i64 &endtime,
					AosTimeGran::E &time_gran,
					AosTime::TimeFormat &format,
					OmnString &timeopr,
					const AosRundataPtr &rdata);

	static u16 mktime(
					const int year,
					const int mon,
					const int day);

	// Chen Ding, 2014/01/29
	// static u64 mktime(
	static i64 mktimeNew(
					const int year, 
					const int mon, 
					const int day,
					const int hour,
					const int min,									
					const int sec);

	static OmnString epochToStr(
					const OmnString &format,
					const char *data,
					const int &datalen);

	// Chen Ding, 2014/01/29
	// static OmnString epochToStr(
	// 				const OmnString &format,
	// 				const u64 &epoch);
	static OmnString epochToStrNew(
					const OmnString &format,
					const i64 &epoch);

	static OmnString convertToLocalTime(
					const OmnString &format, 
					const char *data, 
					const int &datalen);
	
	// Chen Ding, 2014/01/29
	// static u64 temporaryMethod(const OmnString &timeStr);
	static i64 str2EpochTime(const OmnString &timeStr);
	static i32 str2EpochDay(const OmnString &timeStr)
	{
		i64 etime = str2EpochTime(timeStr);
		return etime/eSecondsPerDay;
	}
	static bool str2EpochTime(
					const AosRundataPtr &rdata, 
					const OmnString &timestr, 
					i64 &epoch_time);
	static inline bool str2EpochDay(
					const AosRundataPtr &rdata, 
					const OmnString &timestr, 
					int &epoch_day)
	{
		i64 etime;
		if (!str2EpochTime(rdata, timestr, etime)) return false;
		epoch_day = etime/eSecondsPerDay;
		return true;
	}

	static int getDayOfMonth(const i64 second);
	static int getDayOfWeek(const i64 second);
	static int getEpochMonth(const i64 second);
	static int getEpochYear(const i64 second);
	static int getEpochDay(const i64 second);
	static int getEpochHour(const i64 second);
	static int getEpochWeek(const i64 second);
	static int getHourOfDay(const i64 second);

	static bool calculateTimeToPtime(
					const int number,
					const OmnString &timetype,
					ptime &crt_ptime,
					ptime &cal_ptime,
					const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/29
	// static bool calculateTime(
	// 				const int number,
	// 				const OmnString &timetype,
	// 				ptime &crt_ptime,
	// 				ptime &cal_ptime,
	// 				u64 &crt_epoch,
	// 				u64 &cal_epoch,
	// 				const AosRundataPtr &rdata);
	static bool calculateTimeNew(
					const int number,
					const OmnString &timetype,
					ptime &crt_ptime,
					ptime &cal_ptime,
					i64 &crt_epoch,
					i64 &cal_epoch,
					const AosRundataPtr &rdata);
	static ptime procHour(const AosRundataPtr &rdata, ptime p1, int num);
	static ptime procMinute(const AosRundataPtr &rdata, ptime p1, int num);
	static ptime procSecond(const AosRundataPtr &rdata, ptime p1, int num);
	static ptime procYear(const AosRundataPtr &rdata, date d, int num);
	static ptime procMonth(const AosRundataPtr &rdata, date d, int num);
	static ptime procDay(const AosRundataPtr &rdata, date d, int num);
	static ptime procWeek(const AosRundataPtr &rdata, date d, int num);
	static OmnString PtimeToLocalTime(const AosRundataPtr &rdata, ptime time);
	static OmnString PtimeToStr(const AosRundataPtr &rdata, ptime time);
	// Chen Ding, 2014/01/29
	// static OmnString EpochDayToTime(const AosRundataPtr &rdata, u64 epoch);
	static OmnString EpochDayToTimeNew(const AosRundataPtr &rdata, i64 epoch);
	static OmnString DayOfWeekToString(const AosRundataPtr &rdata, u64 value);

	// Chen Ding, 2014/01/29
	// static OmnString EpochToTime(u64 epoch, const OmnString &format);
	// static OmnString EpochToLocalTime(const AosRundataPtr &rdata, u64 epoch);
	static OmnString EpochToTimeNew(i64 epoch, const OmnString &format);
	static OmnString EpochToLocalTimeNew(const AosRundataPtr &rdata, i64 epoch);

	// Chen Ding, 2014/01/29
	// static OmnString	EpochToTime(u64 epoch)
	static OmnString	EpochToTimeNew(i64 epoch)
	{
		return EpochToTimeNew(epoch, "%Y-%m-%d %H:%M:%S");
	}
	static int getNumDaysInMonth(const int year, const int month);

	// Chen Ding, 2014/02/04
	static bool str2Time(
					const AosRundataPtr &rdata, 
					const OmnString &format,
					const OmnString &str_value,
					i64 &value);
};
#endif

