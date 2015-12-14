////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/DataConverters/JimoDataConverters.h"
#include "DynJimos/DataConverters/JimoDataConvertersShell.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"


static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;
const u32 SIZE = 13;

static OmnString sgMonthJimoUp[SIZE] =            
{
	"",
	"JAN.",
	"FEB.",
	"MSR.",
	"APR.",
	"MAY",
	"JUN.",
	"JUL.",
	"AUG.",
	"SEP.",
	"OCT.",
	"NOV.",
	"DEC."
};


static OmnString sgMonthJimo[SIZE] = 
{
	"",
	"Jan.",
	"Feb.",
	"Mar.",
	"Apr.",
	"May",
	"Jun.",
	"Jul.",
	"Aug.",
	"Sep.",
	"Oct.",
	"Nov.",
	"Dec."
};

static OmnString sgMonthJimoName[SIZE] = 
{
	"",
	"Janurary",
	"Feburary", 
	"March", 
	"April", 
	"May", 
	"June", 
	"July", 
	"August", 
	"September", 
	"October", 
	"Novemeber", 
	"December"
};


AosJimoDataConverters::AosJimoDataConverters(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc)
{
	if (!init(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosJimoDataConverters::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return config(rdata, worker_doc, jimo_doc);
}


bool
AosJimoDataConverters::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	registerMethod("get_epoch_year", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoDataConverters_getEpochYear);

	registerMethod("get_epoch_month", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoDataConverters_getEpochMonth);

	registerMethod("get_epoch_day", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoDataConverters_getEpochDay);

	registerMethod("get_epoch_hour", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoDataConverters_getEpochHour);

	registerMethod("get_epoch_time", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoDataConverters_getEpochTime);

	registerMethod("get_start_time", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoDataConverters_getStartTime);



	sgInited = true;
	sgLock.unlock();
	return true;
}


AosJimoDataConverters::AosJimoDataConverters(
		const AosJimoDataConverters &rhs)
:
AosJimo(rhs),
mFormat(rhs.mFormat),
mStartTime(rhs.mStartTime),
mOutPutFormat(rhs.mOutPutFormat),	
mSep(rhs.mSep),	
mWholeYear(rhs.mWholeYear),
mEpochTime(rhs.mEpochTime),
mSepLen(rhs.mSepLen),	
mYear(rhs.mYear),
mMonth(rhs.mMonth),
mDay(rhs.mDay)
{

}


AosJimoDataConverters::~AosJimoDataConverters()
{
}


bool
AosJimoDataConverters::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	//worker_doc format
	//
	//<worker
	//		input_format=""
	//		called_jimo_objid=xxx"
	//		epoch_time = "xxxxx"
	//		output_datatype="xxx"
	//		starting_time="xxx"
	//		whole_year="false|true"
	//		zky_sep="xxx"
	// />
	
	aos_assert_rr(worker_doc, rdata, false);
	mRundata = rdata;

	mStartTime  = worker_doc->getAttrStr("starting_time"); 
	aos_assert_rr(mStartTime != "", rdata, false);

	mFormat = worker_doc->getAttrStr("input_format");
	aos_assert_rr(mFormat != "", rdata, false);

	mOutPutFormat = worker_doc->getAttrStr("output_datatype");
	aos_assert_rr(mOutPutFormat != "", rdata, false);

	mSep = worker_doc->getAttrStr("zky_sep");
	if (mSep == "") mSepLen = 0;
	mSepLen = mSep.length();

	
	mWholeYear = (worker_doc->getAttrStr("whole_year") == "true") ? true : false;
	
	mEpochTime = worker_doc->getAttrU64("epoch_time", 0);

	return true;
}


OmnString
AosJimoDataConverters::getYearOfYY(
		const int year)
{
	aos_assert_r(year < 100, "");
	time_t timer;
	struct tm *timeinfo;
	OmnString new_year;

	time(&timer);
	timeinfo = localtime(&timer);
	OmnString tmp_year;
	tmp_year << timeinfo->tm_year + 1900;
	int iYear = tmp_year.substr(0, 1).toInt();
	int iYear2 = tmp_year.substr(2, 3).toInt();
	if (year > iYear2)
	{	
		new_year << iYear -1 << year;
		return new_year;
	}
	new_year << iYear;
	if (year < 10) new_year << "0";
	new_year <<  year;
	return new_year;
}

bool
AosJimoDataConverters::setYYMD()
{
	// stacting time format :: yyyymmdd
	
	int startTimeLen = mStartTime.length();
	int startLen = 0;
	int endLen = startLen + 3;
	OmnString year = mStartTime.substr(startLen, endLen);
	mYear = year.toInt(); 

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString month = mStartTime.substr(startLen, endLen);
	mMonth = month.toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString day = mStartTime.substr(startLen, endLen);
	mDay = day.toInt();
	return true;
}


bool
AosJimoDataConverters::setYMD()
{
	// starting time format :: yymmdd
	
	int startTimeLen = mStartTime.length();
	int startLen = 0;
	int endLen = startLen + 1;
	OmnString year = mStartTime.substr(startLen, endLen);
	mYear = year.toInt();
	mYear = getYearOfYY(mYear).toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString month = mStartTime.substr(startLen, endLen);
	mMonth = month.toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString day = mStartTime.substr(startLen, endLen);
	mDay = day.toInt();
	return true;
}


bool
AosJimoDataConverters::setMDYY()
{
	// starting time format :: mmddyyyy
	
	int startTimeLen = mStartTime.length();
	int startLen = 0;
	int endLen = startLen + 1;
	OmnString month = mStartTime.substr(startLen, endLen);
	mMonth = month.toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString day = mStartTime.substr(startLen, endLen);
	mDay = day.toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 3;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString year = mStartTime.substr(startLen, endLen);
	mYear = year.toInt();
	return true;
}


bool
AosJimoDataConverters::setMDY()
{
	// starting time format :: mmddyy
	
	int startTimeLen = mStartTime.length();
	int startLen = 0;
	int endLen = startLen + 1;
	OmnString month = mStartTime.substr(startLen, endLen);
	mMonth = month.toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString day = mStartTime.substr(startLen, endLen);
	mDay = day.toInt();

	startLen = endLen + mSepLen + 1;
	endLen = startLen + 1;
	aos_assert_r(endLen < startTimeLen, false);
	OmnString year = mStartTime.substr(startLen, endLen);
	mYear = year.toInt();
	mYear = getYearOfYY(mYear).toInt();
	return true;
}


u32
AosJimoDataConverters::getMonthIntInArray(
		const OmnString &month,
		OmnString *omnStrArr,
		const u32 arrLen)
{
	for (u32 i = 0; i < arrLen; i++)
	{	
		if (month == omnStrArr[i])
			return i;
	}
	return 0;
}


bool
AosJimoDataConverters::setUSMDY(const int stact)
{
	// starting time format :: <ShortMonthInitCapitalMonth>. dd, yyyy. For instance,Jan. 22, 2013
	// stact  : 0 is Jan. 22, 2013 
	// 			1 is JAN. 22, 2013	
	// 			2 is January 22, 2013

	const char *ch = mStartTime.data();
	int tmp = 0; 	 // if tmp is 0  set mMonth  , 1 set mDay, 2 set mYear 
	int len = 0;
	int startTimeLen = mStartTime.length();
	
	while(startTimeLen > 0)
	{
		if (*ch >= 'A' && *ch <= 'Z' && tmp == 0)
		{
			OmnString month = mStartTime.substr(len, len + 2);
			month << ".";
			if (stact != 2)
			{
				if (stact == 0)
				{

					mMonth = getMonthIntInArray(month, sgMonthJimo, SIZE); 
				}
				if (stact == 1)
				{
					mMonth = getMonthIntInArray(month, sgMonthJimoUp, SIZE);
				}
			}
			if (stact == 2)
			{
				mMonth = getMonthIntInArray(month, sgMonthJimo, SIZE);
				u32 idx = getMonthIntInArray(month, sgMonthJimo, SIZE);
				if (idx > SIZE)
				{
					OmnScreen << "error : idx > sgMonthJimoName length" << endl;	
					return false;
				}
				month = sgMonthJimoName[idx];
				ch = ch + month.length(); 
				len = len + month.length();
				++tmp;
				--startTimeLen;
				continue;
			}
			startTimeLen = startTimeLen + 3;
			ch = ch + 3;
			len = len + 3;
			++tmp;
			--startTimeLen;
			continue;
		}

		if (*ch >= '0' && *ch <= '9' && tmp == 1)
		{
			mDay = mStartTime.substr(len, len+1).toInt();
			ch = ch + 2;
			len = len + 2;
			++tmp;
			--startTimeLen;
			continue;
		}

		if (*ch >= '0' && *ch <= '9' && tmp == 2)
		{
			mYear = mStartTime.substr(len, len+3).toInt();
			break;
		}

		++len;
		++ch;
		--startTimeLen;
	}
	return true;
}


bool
AosJimoDataConverters::setCYMD(const int stact)
{
	// starting time format :: For instance, 2013年06月08日 
	// stact  : 0 is 2013年06月08日 
	//          1 is 2013年06
	//
	const char *ch = mStartTime.data();
	int tmp = 0;     // if tmp is 0  set mYear  , 1 set mMonth, 2 set mDay
	int len = 0;
	int startTimeLen = mStartTime.length();
	while (startTimeLen > 0)
	{
		if (*ch >= '0' && *ch <= '9' && tmp == 0)
		{
			OmnString year = mStartTime.substr(len, len+3);
			mYear = year.toInt();
			len = len + year.length();
			ch = ch + year.length();
			--startTimeLen;
			++tmp;
			continue;
		}
		if (*ch >= '0' && *ch <= '9' && tmp == 1)
		{
			OmnString month = mStartTime.substr(len, len+1);
			mMonth = month.toInt();
			len = len + month.length();
			ch = ch + month.length();
			--startTimeLen;
			++tmp;
			continue;
		}
		if (*ch >= '0' && *ch <= '9' && tmp == 2)
		{
			if (stact == 1)
			{
				mDay = 0;
				break;
			}
			OmnString day = mStartTime.substr(len, len + 1);
			mDay = day.toInt();
			break;
		}
		++len;
		++ch;
		--startTimeLen;
	}
	return true;
}


bool
AosJimoDataConverters::matchFormat()
{
	aos_assert_r(mFormat.length() > 0, false);
	const char *data = mFormat.data();
	aos_assert_r(data, false);
	switch (data[0])
	{
	case 'y':
		 if (mFormat == "yyyymmdd") return setYYMD();
		 if (mFormat == "yymmdd") return setYMD();
		 break;
	
	case 'm':
		 if (mFormat == "mmddyyyy") return setMDYY();
		 if (mFormat == "mmddyy") return 	setMDY();
		 break;

	case 's':
		 if (mFormat == "smcin-ddyyyy") return setUSMDY(0);
		 if (mFormat == "smcn-ddyyyy") return setUSMDY(1);
		 break;
	
	case 'f':
		 if (mFormat == "fmn-ddyyyy") return setUSMDY(2);
		 break;

	case 'c':
		 if (mFormat == "cyyyymmdd") return setCYMD(0);
		 if (mFormat == "cyyyymm") return setCYMD(1);
		 break;

	default:
		 break;
	}
	return false;
}


OmnString
AosJimoDataConverters::toString() const
{
	return "";
}


double
AosJimoDataConverters::diffTime(
		const int year,
		const int month,
		const int day)
{
	time_t timer;
	struct tm y2k;
	double seconds;

	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = (year - 1900); y2k.tm_mon = month; y2k.tm_mday = day;

	time(&timer);  /* get current time; same as: timer = time(NULL)  */

	seconds = difftime(timer,mktime(&y2k));
	return seconds;
}


OmnString
AosJimoDataConverters::matchOutPutFormat()
{
	aos_assert_r(mOutPutFormat.length() > 0, "");
	const char *data = mOutPutFormat.data();
	aos_assert_r(data, "");
	switch (data[0])
	{
	case 'y':
		 if (mOutPutFormat == "yyyymmdd") return getYYMD();
		 if (mOutPutFormat == "yymmdd") return getYMD();
		 break;
	
	case 'm':
		 if (mOutPutFormat == "mmddyyyy") return getMDYY();
		 if (mOutPutFormat == "mmddyy") return 	getMDY();
		 break;

	case 's':
		 if (mOutPutFormat == "smcin-ddyyyy") return getUSMDY(0);
		 if (mOutPutFormat == "smcn-ddyyyy") return getUSMDY(1);
		 break;
	
	case 'f':
		 if (mOutPutFormat == "fmn-ddyyyy") return getUSMDY(2);
		 break;

	case 'c':
		 if (mOutPutFormat == "cyyyymmdd") return getCYMD(0);
		 if (mOutPutFormat == "cyyyymm") return getCYMD(1);
		 break;

 	default:
		 break;
	}
	return "";	
}


bool
AosJimoDataConverters::setU64YMD()
{
	time_t timer;
	struct tm *y2k;
	time(&timer);
	time_t new_time = timer - mEpochTime;
	y2k = localtime(&new_time);	
	mYear = y2k->tm_year + 1900;
	aos_assert_r(mYear, false);

	mMonth = y2k->tm_mon + 1;
	aos_assert_r(mMonth, false);

	mDay = y2k->tm_mday;
	aos_assert_r(mDay, false);
	return true;
}


OmnString
AosJimoDataConverters::getYYMD()
{
	OmnString new_time;
	new_time << mYear << mSep ; 
	if (mMonth < 10) new_time << "0";
	new_time << mMonth << mSep;
	if (mDay < 10) new_time << "0";
	new_time << mDay;
	return new_time;
}


OmnString
AosJimoDataConverters::getYMD()
{
	OmnString new_year;
	OmnString new_time;
	new_year << mYear;
	new_year = new_year.substr(2, new_year.length()-1);
	new_time << new_year << mSep ; 
	if (mMonth < 10) new_time << "0";
	new_time << mMonth << mSep;
	if (mDay < 10) new_time << "0";
	new_time << mDay;
	return new_time;
}


OmnString
AosJimoDataConverters::getMDYY()
{
	OmnString new_time;
	if (mMonth < 10) new_time << "0";
	new_time << mMonth << mSep;
	if (mDay < 10) new_time << "0";
	new_time << mDay << mSep;
	new_time << mYear; 
	return new_time;
}


OmnString
AosJimoDataConverters::getMDY()
{
	OmnString new_year;
	OmnString new_time;
	new_year << mYear;
	if (mMonth < 10) new_time << "0";
	new_time << mMonth << mSep;
	if (mDay < 10) new_time << "0";
	new_time << mDay << mSep;
	new_year = new_year.substr(2, new_year.length()-1);
	new_time << new_year;
	return new_time;
}


OmnString
AosJimoDataConverters::getUSMDY(const int stact)
{
	// starting time format :: <ShortMonthInitCapitalMonth>. dd, yyyy. For instance,Jan. 22, 2013
	// stact  : 0 is Jan. 22, 2013 
	//          1 is JAN. 22, 2013  
	//          2 is January 22, 2013
	OmnString new_time;
	OmnString month;
	aos_assert_r(mMonth < SIZE, "");

	if (stact == 0) month =	sgMonthJimo[mMonth];
	if (stact == 1) month =	sgMonthJimoUp[mMonth];
	if (stact == 2) month =	sgMonthJimoName[mMonth];
	new_time << month << " " << mDay << ", " << mYear;
	return new_time;
}


OmnString
AosJimoDataConverters::getCYMD(const int stact)
{
	// starting time format :: For instance, 2013年06月08日 
	// stact  : 0 is 2013年06月08日 
	//          1 is 2013年06
	OmnString new_time;
	new_time << mYear << "年"; 
	if (mMonth < 10) new_time << "0";
	new_time << mMonth << "月";

	if (stact == 0)
	{
		if (mDay < 10) new_time << "0";
		new_time << mDay << "日";
		return new_time;
	}
	return new_time;
}


int
AosJimoDataConverters::getEpochYear()
{
	bool rslt = matchFormat();
	aos_assert_r(rslt, 0);
	rslt = false;

	u32 year, month, day;
	year = month = day = 0;
	rslt = getLocalYMD(year, month, day);
	aos_assert_r(rslt, false);

	OmnScreen << mYear << " " << mMonth << " " << mDay << endl; 
	OmnScreen << "--------Mmonth:: " << mMonth << endl;
	OmnScreen << "--------month:: " << month << endl;
	
	int diffYear= year - mYear;
	if (mWholeYear && month < mMonth) --diffYear;
	mRundata->setInt64Value(diffYear);
	return diffYear;
}


int 
AosJimoDataConverters::getEpochMonth()
{
	bool rslt = matchFormat();
	aos_assert_r(rslt, 0);
	rslt = false;

	u32 year, month, day;
	year = month = day = 0;
	rslt = getLocalYMD(year, month, day);
	aos_assert_r(rslt, false);
	
	int diffYear = year - mYear;
	int diffMonth = diffYear * 12 + month -mMonth;
	return diffMonth;
}

bool
AosJimoDataConverters::getLocalYMD(
		u32 &year,
		u32 &month,
		u32 &day)
{
	time_t timer;
	struct tm *timeinfo;
	
	time(&timer);
	timeinfo = localtime(&timer);
	year = timeinfo->tm_year + 1900;
	aos_assert_r(year, false);

	month = timeinfo->tm_mon + 1;
	aos_assert_r(month, false);

	day = timeinfo->tm_mday;
	aos_assert_r(day, false);

	return true;
}


int
AosJimoDataConverters::getEpochDay()
{
	bool rslt = matchFormat();
	aos_assert_r(rslt, 0);

	double seconds = diffTime(mYear, mMonth, mDay);
	return (int)(seconds/3600/24);
}


int
AosJimoDataConverters::getEpochHour()
{
	bool rslt = matchFormat();
	aos_assert_r(rslt, 0);

	double seconds = diffTime(mYear, mMonth, mDay);
	return (int)(seconds/3600);
}


int
AosJimoDataConverters::getEpochTime()
{
	bool rslt = matchFormat();
	aos_assert_r(rslt, 0);

	double seconds = diffTime(mYear, mMonth, mDay);
	return (int)(seconds);
}


OmnString
AosJimoDataConverters::getStartTime()
{
	bool rslt = setU64YMD();
	aos_assert_r(rslt, "");
	return matchOutPutFormat();
}


bool 
AosJimoDataConverters::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoDataConverters::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoDataConverters::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosJimoDataConverters(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


void * 
AosJimoDataConverters::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}


