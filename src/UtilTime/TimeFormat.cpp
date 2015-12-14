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
// This class assumes a u64 0 represents Year eStartYear. 
// Given a string: yyyymmddhhmmss, its u64 representation should be:
// 		u64 second = yyyy * eNumSecondsPerYear + 
//
// Modification History:
// 06/06/2012 Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#include "UtilTime/TimeFormat.h"

#include "Porting/GetTime.h"
#include "Util/UtUtil.h"
#include "Util/HashUtil.h"
#include "Util/OmnNew.h"
#include "UtilTime/TFDayOfMonth.h"
#include "UtilTime/TFDayOfWeek.h"
#include "UtilTime/TFEpochDay.h"
#include "UtilTime/TFEpochHour.h"
#include "UtilTime/TFEpochMonth.h"
#include "UtilTime/TFEpochYear.h"
#include "UtilTime/TFEpochWeek.h"
#include "UtilTime/TFEpochTime.h"
#include "UtilTime/TFHourOfDay.h"
#include "UtilTime/TFYYMMDD.h"
#include "UtilTime/TFYYMMDDHHMMSS.h"
#include "UtilTime/TFYYYYDOTMM.h"
#include "UtilTime/TFYYYYMMDD.h"
#include "UtilTime/TFYYYYMMDDHHMMSS.h"
#include "UtilTime/TFYYYY_MM_DDHHMMSS.h"
#include "UtilTime/TFYYYY_MM_DDHHMM.h"
#include "UtilTime/TFMM_DD_YYYY_HH_MM.h"
#include "UtilTime/TFMM-DD-YYYY.h"
#include "UtilTime/TimeUtil.h"
#include "UtilTime/TFYYYYMM.h"


static AosStr2U32_t 	sgNameMap;
static OmnString		sgNames[AosTimeFormat::eMax];
static bool				sgInited = false;
static OmnMutex			sgLock;
AosTimeFormatPtr		AosTimeFormat::smFormats[AosTimeFormat::eMax];


AosTimeFormat::AosTimeFormat(
		const OmnString &name,
		const E code,
		const bool flag)
{
	if (!flag) return;
	bool rslt = registerFormat(name, code);
	if (!rslt) OmnThrowException("failed registering format");
}


bool
AosTimeFormat::registerFormat(const OmnString &name, const E code) 
{
	aos_assert_r(name != "", false);
	aos_assert_r(code > eInvalid && code < eMax, false);
	aos_assert_r(sgNames[code] == "", false);

	AosStr2U32Itr_t itr = sgNameMap.find(name);
	aos_assert_r(itr == sgNameMap.end(), false);

	sgNames[code] = name;
	sgNameMap[name] = code;
	return true;
}


bool
AosTimeFormat::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	smFormats[eDayOfMonth]			= OmnNew AosTFDayOfMonth(true);
	smFormats[eDayOfWeek]			= OmnNew AosTFDayOfWeek(true);
	smFormats[eEpochDay]			= OmnNew AosTFEpochDay(true);
	smFormats[eEpochHour]			= OmnNew AosTFEpochHour(true);
	smFormats[eEpochMonth]			= OmnNew AosTFEpochMonth(true);
	smFormats[eEpochYear]			= OmnNew AosTFEpochYear(true);
	smFormats[eEpochWeek]			= OmnNew AosTFEpochWeek(true);
	smFormats[eEpochTime]			= OmnNew AosTFEpochTime(true);
	smFormats[eHourOfDay]			= OmnNew AosTFHourOfDay(true);
	smFormats[eYYMMDD] 				= OmnNew AosTFYYMMDD(true);
	smFormats[eYYMMDDHHMMSS] 		= OmnNew AosTFYYMMDDHHMMSS(true);
	smFormats[eYYYYMMDD] 			= OmnNew AosTFYYYYMMDD(true);
	smFormats[eYYYYMMDDHHMMSS] 		= OmnNew AosTFYYYYMMDDHHMMSS(true);
	smFormats[eMMDDYYYY] 			= OmnNew AosTFMMDDYYYY(true);
	smFormats[eYYYY_MM_DDHHMMSS] 	= OmnNew AosTFYYYY_MM_DDHHMMSS(true);
	smFormats[eYYYY_MM_DDHHMM] 		= OmnNew AosTFYYYY_MM_DDHHMM(true);
	smFormats[eMM_DD_YYYY_HH_MM] 	= OmnNew AosTFMM_DD_YYYY_HH_MM(true);
	smFormats[eYYYYMM] 				= OmnNew AosTFYYYYMM(true);
	smFormats[eYYYYDOTMM] 			= OmnNew AosTFYYYYDOTMM(true);

	bool status = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		if (sgNames[i] == "")
		{
			OmnAlarm << "Name not added: " << i << enderr;
			status = false;
		}

		AosStr2U32Itr_t itr = sgNameMap.find(sgNames[i]);
		if (itr == sgNameMap.end())
		{
			OmnAlarm << "Name not added: " << sgNames[i] << enderr;
			status = false;
		}
	}

	sgInited = true;
	sgLock.unlock();
	return status;
}

	
AosTimeFormat::E
AosTimeFormat::toEnum(const OmnString &name)
{
	if (!sgInited) init();

	AosStr2U32Itr_t itr = sgNameMap.find(name);
	if (itr == sgNameMap.end()) return eInvalid;
	return (E)itr->second;
}


OmnString
AosTimeFormat::toString(const E code)
{
	if (code <= eInvalid || code >= eMax) return AOSTIMEFORMAT_INVALID;
	if (!sgInited) init();
	return sgNames[code];
}
	

AosTimeFormatPtr
AosTimeFormat::getFormat(const E code)
{
	if (code <= eInvalid || code >= eMax) return 0;
	if (!sgInited) init();
	return smFormats[code];
}


int
AosTimeFormat::str2EpochDay(const char *data, const int len) const
{
	OmnNotImplementedYet;
	return 0;
}


int
AosTimeFormat::str2EpochHour(const char *data, const int len) const
{
	OmnNotImplementedYet;
	return 0;
}


int
AosTimeFormat::str2EpochMonth(const char *data, const int len) const
{
	OmnNotImplementedYet;
	return 0;
}


int
AosTimeFormat::str2EpochYear(const char *data, const int len) const
{
	OmnNotImplementedYet;
	return 0;
}


int
AosTimeFormat::str2EpochWeek(const char *data, const int len) const
{
	OmnNotImplementedYet;
	return 0;
}


i64
AosTimeFormat::str2EpochTime(const char *data, const int len) const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosTimeFormat::convert(
		const char *data, 
		const int len,
		const E target_format,
		AosValueRslt &value, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTimeFormat::check(const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return false;
}
	 

int
AosTimeFormat::getDataLen(const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, -1);
	return obj->getDataLen();
}


int
AosTimeFormat::strToEpochDay(const OmnString &timestr, const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, 0);
	return obj->str2EpochDay(timestr.getBuffer(), timestr.length());
}


int
AosTimeFormat::strToEpochHour(const OmnString &timestr, const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, 0);
	return obj->str2EpochHour(timestr.getBuffer(), timestr.length());
}


int
AosTimeFormat::strToEpochMonth(const OmnString &timestr, const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, 0);
	return obj->str2EpochMonth(timestr.getBuffer(), timestr.length());
}


int
AosTimeFormat::strToEpochYear(const OmnString &timestr, const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, 0);
	return obj->str2EpochYear(timestr.getBuffer(), timestr.length());
}


int
AosTimeFormat::strToEpochWeek(const OmnString &timestr, const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, 0);
	return obj->str2EpochWeek(timestr.getBuffer(), timestr.length());
}


i64
AosTimeFormat::strToEpochTime(const OmnString &timestr, const E format)
{
	AosTimeFormatPtr obj = getFormat(format);
	aos_assert_r(obj, 0);
	return obj->str2EpochTime(timestr.getBuffer(), timestr.length());
}


bool
AosTimeFormat::convert(
		const char *data, 
		const int len,
		const E orig_format,
		const E target_format,
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	AosTimeFormatPtr obj = getFormat(orig_format);
	aos_assert_r(obj, false);
	return obj->convert(data, len, target_format, value, rdata);
}
