////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GetTime.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_GetTime_h
#define Omn_Porting_GetTime_h

#include "Util/Locale.h"
#include "Util/String.h"

static OmnString sgMonth[13] = 
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

static OmnString sgMonthName[13] = 
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


// enum AosLocale
// {
// 	eAosLocale_Invalid,
// 
// 	eAosLocale_USA,
// 	eAosLocale_China
// };

// static AosLocale::E sgAosLocale = AosLocale::eUSA;

// extern inline AosLocale::E AosLocaleToCode(const OmnString &name)
// {
// 	if (name == "usa") return eAosLocale_USA;
// 	if (name == "china") return eAosLocale_China;
// 	return eAosLocale_Invalid;
// }

/*
inline void AosSetLocale(const OmnString &name)
{
	sgAosLocale = AosLocale::toEnum(name);
}

inline AosLocale::E AosGetLocale()
{
	return sgAosLocale;
}
*/


#ifdef OMN_PLATFORM_UNIX
#include "Util/String.h"
#include <time.h>

// The struct tm has the following members:
// tm_sec	seconds after the minute		[0, 61]
// tm_min	minutes after the hour			[0, 59]
// tm_hour	hours since midnight			[0, 23]
// tm_mday	day of the month				[1, 31]
// tm_mon	months since January			[0, 11]
// tm_year	years since 1900				
// tm_yday	days since January 1			[0-365]
// tm_wday	days since Sunday				[0-6]
// tm_isdst	Daylight Saving Time flag


inline int AosGetEpoch(
		const char *year,
		const char *month,
		const char *day,
		const char *hour,
		const char *minute,
		const char *second, 
		OmnString &errmsg)
{
	if (!year)
	{
		errmsg = "Missing year!";
		return -1;
	}

	int yy = atoi(year);
	if (yy < 1900)
	{
		errmsg = "Year incorrect: ";
		errmsg << year;
		return -1;
	}
	yy -= 1900;

	// Month: [0, 11]
	int mon = (month)?atoi(month):0;
	if (mon < 0 || mon > 11)
	{
		errmsg = "Month incorrect: ";
		errmsg << month;
		return -1;
	}

	// Day: [1, 31]
	int mday = (day)?atoi(day):1;
	if (mday < 1 || mday > 31)
	{
		errmsg = "day incorrect: ";
		errmsg << day;
		return -1;
	}

	// Hour: [0, 23]
	int hh = (hour)?atoi(hour):0;
	if (hh < 0 || hh > 23)
	{
		errmsg = "Hour incorrect: ";
		errmsg << hh;
		return -1;
	}

	// Minute: [0, 59]
	int min = (minute)?atoi(minute):0;
	if (min < 0 || min > 59)
	{
		errmsg = "Minute incorrect: ";
		errmsg << minute;
		return -1;
	}

	// Second: [0, 59]
	int sec = (second)?atoi(second):0;
	if (sec < 0 || sec > 59)
	{
		errmsg = "Second incorrect: ";
		errmsg << second;
		return -1;
	}

	struct tm theTime;
	theTime.tm_year = yy;
	theTime.tm_mon = mon;
	theTime.tm_mday = mday;
	theTime.tm_hour = hh;
	theTime.tm_min = min;
	theTime.tm_sec = sec;

	time_t tt = mktime(&theTime);
	return tt;
}


inline int AosGetEpochTime(
		const int yy, 
		const int mon,
		const int mday,
		const int hh,
		const int min,
		const int sec)
{
	struct tm theTime;
	theTime.tm_year = yy;
	theTime.tm_mon = mon;
	theTime.tm_mday = mday;
	theTime.tm_hour = hh;
	theTime.tm_min = min;
	theTime.tm_sec = sec;

	time_t tt = mktime(&theTime);
	return tt;
}


OmnString inline AosGetSystemYear()
{
	time_t tt = time(0);
	struct tm theTime = {0};
	localtime_r(&tt, &theTime);
	OmnString str;
	str << "20" << theTime.tm_year - 100;
	return str;
}


OmnString inline AosGetSystemDayOfMonth()
{
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);
	OmnString str;
	str << theTime.tm_mday;	// [1, 31]
	return str;
}


OmnString inline AosGetSystemDayOfWeek()
{
	time_t tt = time(0);
	struct tm theTime = {0};
	localtime_r(&tt, &theTime);
	OmnString str;
	str << theTime.tm_wday;	// [0, 6] (Sunday = 0)
	return str;
}


OmnString inline AosGetSystemMonth()
{
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);
	OmnString str;
	str << theTime.tm_mon + 1; 	// [0, 11]
	return str;
}


extern bool OmnParseTimeStr(
		const AosLocale::E loc,
		const char *timestr,
		char *year,
		char *month,
		char *day);

inline void OmnGetTime(
		OmnString &year,
		OmnString &month,
		OmnString &day, 
		OmnString &hour,
		OmnString &minute)
{
	time_t tt = time(0);
	struct tm theTime = {0};
	localtime_r(&tt, &theTime);
	OmnString str;

	int yy = theTime.tm_year - 100 + 2000;
	
	year << yy;
	month << theTime.tm_mon+1;
	day << theTime.tm_mday;
	hour << theTime.tm_hour;
	minute << theTime.tm_min;
}


inline OmnString OmnGetTime(
		const AosLocale::E loc, 
		char *year,
		char *month,
		char *day)
{
	// 
	// It returns a string of the format:
	// 	MMDDYY_HHMMSS
	//
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);
	OmnString str;

	int yy = theTime.tm_year - 100 + 2000;
	
	sprintf(year, "%04d", yy); 					year[4] = 0;
	sprintf(month, "%02d", theTime.tm_mon+1);	month[2] = 0;
	sprintf(day, "%02d", theTime.tm_mday);		day[2] = 0;

	char buff[30];
	switch (loc)
	{
	case AosLocale::eChina:
		 // YYYY-MM-DD HH:MM:SS 
		 sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d", 
			yy, 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 buff[19] = 0;
		 if (buff[5] == ' ') buff[5] = '0';
		 if (buff[8] == ' ') buff[8] = '0';
		 if (buff[11] == ' ') buff[11] = '0';
		 if (buff[14] == ' ') buff[14] = '0';
		 if (buff[17] == ' ') buff[17] = '0';
		 return buff;

	case AosLocale::eUSA:
	default:
		 // MM-DD-YYYY HH:MM:SS 
		 sprintf(buff, "%02d-%02d-%04d %02d:%02d:%02d", 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			yy, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 if (buff[0] == ' ') buff[0] = '0';
		 if (buff[3] == ' ') buff[3] = '0';
		 if (buff[11] == ' ') buff[11] = '0';
		 if (buff[14] == ' ') buff[14] = '0';
		 if (buff[17] == ' ') buff[17] = '0';
		 buff[19] = 0;
		 return buff;
	}
}


inline OmnString OmnGetTime(const AosLocale::E loc = AosLocale::eUSA)
{
	// 
	// It returns a string of the format:
	// 	MMDDYY_HHMMSS
	//
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);
	OmnString str;

	int year = theTime.tm_year - 100 + 2000;
	
	char buff[30];
	switch (loc)
	{
	case AosLocale::eChina:
		 // YYYY-MM-DD HH:MM:SS 
		 sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d", 
			year, 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 buff[19] = 0;
		 return buff;

	case AosLocale::eUSA:
	default:
		 // MM-DD-YYYY HH:MM:SS 
		 sprintf(buff, "%02d-%02d-%04d %02d:%02d:%02d", 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			year, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 buff[19] = 0;
		 return buff;
	}
}


// Ken Lee, 2013/08/26
inline OmnString OmnGetTime(const OmnString &fmt)
{
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	int year = theTime.tm_year - 100 + 2000;
	
	char buff[30];
	if (fmt == "YYYY-MM-DD hh:mm:ss")
	{
		 // YYYY-MM-DD HH:MM:SS 
		 sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d", 
			year, 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 buff[19] = 0;
		 return buff;
	}
	else if (fmt == "MM-DD-YYYY hh:mm:ss")
	{
		 // MM-DD-YYYY HH:MM:SS 
		 sprintf(buff, "%02d-%02d-%04d %02d:%02d:%02d", 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			year, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 buff[19] = 0;
		 return buff;
	}
	else if (fmt == "YYYYMMDDhhmmss")
	{
		 sprintf(buff, "%04d%02d%02d%02d%02d%02d", 
			year, 
			theTime.tm_mon+1,
			theTime.tm_mday, 
			theTime.tm_hour,
			theTime.tm_min,
			theTime.tm_sec);
		 buff[14] = 0;
		 return buff;
	}
	return "";
}


OmnString inline OmnGetMDY()
{
	// 
	// It returns mm/dd/yyyy
	//
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);
	OmnString str;

	str << theTime.tm_mon + 1 
		<< "/" << theTime.tm_mday
	    << "/200" << theTime.tm_year - 100;
	return str;
}


void inline OmnGetTimeCharStr(char *buf)
{
	// 
	// It returns a string of the format:
	// 	MMDDYY_HHMMSS
	// The caller must allocate enough space for this function.
	//
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	sprintf(buf, "200%d-%02d-%02d %02d:%02d:%02d", 
		theTime.tm_year-100, 
		theTime.tm_mon+1,
		theTime.tm_mday,
		theTime.tm_hour,
		theTime.tm_min,
		theTime.tm_sec);
}


inline OmnString AosGetTime_yyyymmdd()
{
	// It returns a string of the format:
	// 	yyyymmdd
	time_t tt = time(0);
	struct tm theTime = {0};
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%04d%02d%02d", 
		theTime.tm_year-100, 
		theTime.tm_mon+1,
		theTime.tm_mday);
	return buff;
}


inline OmnString AosGetTime_yymmdd()
{
	// It returns a string of the format:
	// 	yyyymmdd
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%02d%02d%02d", 
		theTime.tm_year-100, 
		theTime.tm_mon+1,
		theTime.tm_mday);
	return buff;
}


inline OmnString AosGetTime_mmddyyyy()
{
	// It returns a string of the format:
	// 	yyyymmdd
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%02d%02d%04d", 
		theTime.tm_mon+1,
		theTime.tm_mday,
		theTime.tm_year-100);
	return buff;
}


inline OmnString AosGetTime_mmddyy()
{
	// It returns a string of the format:
	// 	yyyymmdd
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%02d%02d%02d", 
		theTime.tm_mon+1,
		theTime.tm_mday,
		theTime.tm_year-100);
	return buff;
}


/*
inline OmnString AosGetTime_mmddyyyy()
{
	// It returns a string of the format:
	// 	"JAN. dd, 2013"
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%s %02d, %04d", 
		sgMonthCapital[theTime.tm_mon],
		theTime.tm_mday,
		theTime.tm_year-100);
	return buff;
}
*/


inline void AosGetHourMinuteSec(int &hour, int &minute, int &sec)
{
	time_t tt = time(0);
	struct tm theTime = {0};
	localtime_r(&tt, &theTime);
	hour = theTime.tm_hour;
	minute  = theTime.tm_min;
	sec = theTime.tm_sec;
}


inline OmnString AosGetTime_Monthddyyyy()
{
	// It returns a string of the format:
	// 	"Jan. dd, 2013"
	time_t tt = time(0);
	struct tm theTime = {0};
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%s %02d, %04d", 
		sgMonth[theTime.tm_mon].data(),
		theTime.tm_mday,
		theTime.tm_year-100);
	return buff;
}


inline OmnString AosGetTime_MonthNameddyyyy()
{
	// It returns a string of the format:
	// 	"January dd, 2013"
	time_t tt = time(0);
	struct tm theTime = {0}; 
	localtime_r(&tt, &theTime);

	char buff[100];
	sprintf(buff, "%s %02d, %04d", 
		sgMonthName[theTime.tm_mon].data(),
		theTime.tm_mday,
		theTime.tm_year-100);
	return buff;
}


#elif OMN_PLATFORM_MICROSOFT
#include <time.h>


inline OmnString OmnGetTime()
{
	// 
	// It returns a string of the format:
	// 	MMDDYY_HHMMSS
	//
	time_t tt = time(0);
	struct tm theTime = localtime_r(&tt);
	OmnString str;

	str << "200" << theTime.tm_year - 100
		<< "-" << theTime.tm_mon + 1
		<< "-" << theTime.tm_mday
		<< " " << theTime.tm_hour << ":"
		<< theTime.tm_min << ":"
		<< theTime.tm_sec;

	return str;
}


void inline OmnGetTimeCharStr(char *buf)
{
	// 
	// It returns a string of the format:
	// 	MMDDYY_HHMMSS
	// The caller must allocate enough space for this function.
	//
	time_t tt = time(0);
	struct tm theTime = localtime_r(&tt);

	sprintf(buf, "200%d-%02d-%02d %02d:%02d:%02d", 
		theTime.tm_year-100, 
		theTime.tm_mon+1,
		theTime.tm_mday,
		theTime.tm_hour,
		theTime.tm_min,
		theTime.tm_sec);
}

#endif

#endif




