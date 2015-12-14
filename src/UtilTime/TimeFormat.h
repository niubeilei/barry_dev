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
// 06/06/2012 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_TimeFormat_h
#define Omn_UtilTime_TimeFormat_h

#define AOSTIMEFORMAT_INVALID			"invalid"
#define AOSTIMEFORMAT_DAYOFMONTH		"dayofmonth"
#define AOSTIMEFORMAT_DAYOFWEEK			"dayofweek"
#define AOSTIMEFORMAT_EPOCH_DAY			"epochday"
#define AOSTIMEFORMAT_EPOCH_HOUR		"epochhour"
#define AOSTIMEFORMAT_EPOCH_MONTH		"epochmonth"
#define AOSTIMEFORMAT_EPOCH_YEAR		"epochyear"
#define AOSTIMEFORMAT_EPOCH_WEEK		"epochweek"
#define AOSTIMEFORMAT_EPOCH_TIME		"epochtime"
#define AOSTIMEFORMAT_HOUROFDAY			"hourofday"
#define AOSTIMEFORMAT_YYMMDD			"yymmdd"
#define AOSTIMEFORMAT_YYMMDDHHMMSS		"yymmddhhmmss"
#define AOSTIMEFORMAT_YYYYMMDD			"yyyymmdd"
#define AOSTIMEFORMAT_YYYYMMDDHHMMSS	"yyyymmddhhmmss"
#define AOSTIMEFORMAT_MMDDYYYY			"mm-dd-yyyy"
#define AOSTIMEFORMAT_YYYY_MM_DDHHMMSS  "yyyy-mm-dd hh:mm:ss"
#define AOSTIMEFORMAT_YYYY_MM_DDHHMM	"yyyy-mm-dd hh:mm"
#define AOSTIMEFORMAT_MM_DD_YYYY_HH_MM	"mm/dd/yyyy hh:mm"
#define AOSTIMEFORMAT_YYYYDOTMM			"yyyy.mm"
//#define AOSTIMEFORMAT_MMDDYYYY		"mmddyyyy"
#define AOSTIMEFORMAT_YYYYMM			"yyyymm"


#define AOSTIMEFORMAT_HOURS_PRE_DAY			24;
#define AOSTIMEFORMAT_SECONDS_PRE_DAY		(60 * 60 * 24);
#define AOSTIMEFORMAT_SECONDS_PRE_MINUTE	60;
#define AOSTIMEFORMAT_SECONDS_PRE_HOUR		(60 * 60);
#define AOSTIMEFORMAT_SECONDS_PRE_WEEK		(7 * 24 * 60 * 60);


#include "UtilTime/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"


class AosTimeFormat : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
	{
		eInvalid,

		eDayOfMonth,
		eDayOfWeek,			// u16,	days since 1970.1.1
		eEpochDay,			// u16,	days since 1970.1.1
		eEpochHour,			// u32,	days since 1970.1.1
		eEpochMonth,		// u16,	days since 1970.1.1
		eEpochYear,			// u16,	days since 1970.1.1
		eEpochWeek,			// u16,	days since 1970.1.1
		eEpochTime,			// u32, seconds since 1970.1.1 0:0:0
		eHourOfDay,			// u16,	days since 1970.1.1
		eYYMMDD,
		eYYMMDDHHMMSS,
		eYYYYDOTMM,
		eYYYYMMDD,
		eYYYYMMDDHHMMSS,
		eYYYY_MM_DDHHMMSS,
		eYYYY_MM_DDHHMM,
		eMM_DD_YYYY_HH_MM,
		eMMDDYYYY,
		eYYYYMM,

		eMax
	};

private:
	static AosTimeFormatPtr		smFormats[eMax];

public:
	AosTimeFormat(const OmnString &name, const E code, const bool flag);

	virtual int getDataLen() const = 0;
	virtual int str2EpochDay(const char *data, const int len) const;
	virtual int str2EpochHour(const char *data, const int len) const;
	virtual int str2EpochMonth(const char *data, const int len) const;
	virtual int str2EpochYear(const char *data, const int len) const;
	virtual int str2EpochWeek(const char *data, const int len) const;
	virtual i64 str2EpochTime(const char *data, const int len) const;
	virtual bool convert(
						const char *data, 
						const int len,
						const E target_format,
						AosValueRslt &value,
						const AosRundataPtr &rdata) const;
	virtual bool check(const AosRundataPtr &rdata) const;

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &name);
	static OmnString toString(const E code);
	static AosTimeFormatPtr getFormat(const E code);

	static int getDataLen(const E format);
	static int strToEpochDay(const OmnString &timestr, const E format);
	static int strToEpochHour(const OmnString &timestr, const E format);
	static int strToEpochMonth(const OmnString &timestr, const E format);
	static int strToEpochYear(const OmnString &timestr, const E format);
	static int strToEpochWeek(const OmnString &timestr, const E format);
	static i64 strToEpochTime(const OmnString &timestr, const E format);
	static bool convert(
			const char *data, 
			const int len,
			const E orig_format,
			const E target_format,
			AosValueRslt &value,
			const AosRundataPtr &rdata);

	static bool init();

private:
	bool registerFormat(const OmnString &name, const E code);
};
#endif

