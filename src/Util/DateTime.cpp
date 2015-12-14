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
// 06/16/2015	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Util/DateTime.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/exception/all.hpp>
using namespace boost::gregorian;

AosDateTime::AosDateTime()
{
	mFormat = "%Y-%m-%d %H:%M:%S";
	mPtime = ptime();
}

AosDateTime::AosDateTime(const time_t tt, const string &format)
{
	try
	{
		mFormat = format; 
		if (mFormat == "")
			mFormat = "%Y-%m-%d %H:%M:%S";
		struct tm tm_tmp;
		struct tm *tm = localtime_r(&tt, &tm_tmp);
		if (tm)
		{
			mPtime = ptime_from_tm(*tm);
		}
		else
		{
			AosDateTime();
		}
	}
	catch (boost::exception &e)
	{
		//OmnScreen << diagnostic_information(e) << endl;
		AosDateTime();
	}
}


AosDateTime::AosDateTime(const string &timestr, const string &format)
{
	mFormat = format;
	if (mFormat == "") 
		mFormat = "%Y-%m-%d %H:%M:%S";
	// convert time string to ptime
	if (timestr == "") 
	{
		mPtime = ptime();
		return;
	}

	struct tm tm;
	memset(&tm, 0, sizeof(struct tm));
	strptime(timestr.data(), mFormat.data(), &tm);
	try
	{
		mPtime = ptime_from_tm(tm);
	}
	catch (boost::exception &e)
	{
		//OmnScreen << diagnostic_information(e) << endl;
		mPtime = ptime();
	}
}


AosDateTime::AosDateTime(const AosDateTime &rhs)
{
	aos_assert(rhs.mFormat != "");
	aos_assert(rhs.mFormat != " ");
	mFormat = rhs.mFormat;
	mPtime = rhs.mPtime;
}

bool 			
AosDateTime::setFormat(const OmnString &fmt)
{
	mFormat = (fmt == "") ? "%Y-%m-%d %H:%M:%S" : fmt;
	mFormat = fmt;
	return true;
}

	
string 			
AosDateTime::getFormat() const
{
	return mFormat;
}

AosDateTime::AosDateTime(const ptime &time, const string &format)
{
	mPtime  = time;
	mFormat = (format == "") ? "%Y-%m-%d %H:%M:%S" : format;
}


AosDateTime::~AosDateTime()
{
}


string
AosDateTime::toString() const 
{
	return toString(mFormat);
}


string
AosDateTime::toString(const string &fmt) const 
{
	if ( isNotADateTime() ) return "invalid";

	try
	{
		struct tm tm = to_tm(mPtime);
		char buf[255];
		bzero(buf, sizeof(buf));
		strftime(buf, sizeof(buf), fmt.data(), &tm);
		return string(buf, strlen(buf));
	}
	catch (...)
	{
		return "invalid";
	}
}


AosDateTime&
AosDateTime::operator = (const AosDateTime &rhs)
{
	mFormat = rhs.mFormat;
	mPtime = rhs.mPtime;
	return *this;
}


//AosTimeDuration
int64_t
AosDateTime::operator - (const AosDateTime &rhs)
{
	//if ( isNotADateTime() ) return AosTimeDuration();
	if ( isNotADateTime() ) return 0;

	time_duration td = mPtime - rhs.mPtime;
	//return AosTimeDuration(td);
	return td.total_seconds();
}


AosDateTime		
AosDateTime::operator + (const AosTimeDuration &td)
{
	if ( isNotADateTime() ) return AosDateTime(*this);
	ptime pt = mPtime + td.mTD;
	return AosDateTime(pt, mFormat);
}


AosDateTime		
AosDateTime::operator - (const AosTimeDuration &td)
{
	if ( isNotADateTime() ) return AosDateTime(*this);

	ptime pt = mPtime - td.mTD;
	return AosDateTime(pt, mFormat);
}


bool
AosDateTime::operator == (const AosDateTime &rhs) const
{
	return mPtime == rhs.mPtime;
}


bool 
AosDateTime::operator != (const AosDateTime &rhs) const
{
	return mPtime != rhs.mPtime;
}


bool 
AosDateTime::operator < (const AosDateTime &rhs) const
{
	return mPtime < rhs.mPtime;
}


bool 
AosDateTime::operator <= (const AosDateTime &rhs) const
{
	return mPtime <= rhs.mPtime;
}


bool 
AosDateTime::operator >  (const AosDateTime &rhs) const
{
	return mPtime > rhs.mPtime;
}


bool 
AosDateTime::operator >= (const AosDateTime &rhs) const
{
	return mPtime >= rhs.mPtime;
}


time_t 				
AosDateTime::to_time_t() const
{
	if (isNotADateTime()) 
	{
		OmnAlarm << "Current DateTime Object is invalid" << enderr;
		return 0;
	}
	
	//struct tm tm = toTM();
	struct tm tm = to_tm(mPtime);
	return mktime(&tm);
}

