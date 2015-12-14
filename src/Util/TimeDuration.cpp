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
#include "Util/TimeDuration.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

#include "boost/date_time/gregorian/gregorian.hpp"
using namespace boost::gregorian;



AosTimeDuration::AosTimeDuration(i64 hours,
		i64 minutes, i64 seconds, i64 nanoseconds)
{	
	mTD = time_duration(hours, minutes, seconds, nanoseconds);	
}

AosTimeDuration::AosTimeDuration(const AosTimeDuration &rhs)
{
	mTD = time_duration(rhs.mTD.hours(), rhs.mTD.minutes(), rhs.mTD.seconds());
	//rhs.mTD.milliseconds(), rhs.mTD.microseconds(), rhs.mTD.nanoseconds());
}


AosTimeDuration::AosTimeDuration(const time_duration &td)
{
	mTD = time_duration(td.hours(), td.minutes(), td.seconds());
}


AosTimeDuration::~AosTimeDuration()
{
}


string
AosTimeDuration::toString() const
{
	return to_simple_string(mTD);
}


AosTimeDuration
AosTimeDuration::fromString(const string &value)
{
	time_duration td = duration_from_string(value);
	return AosTimeDuration(td);
}


AosTimeDuration&
AosTimeDuration::operator = (const AosTimeDuration &rhs)
{
	mTD = rhs.mTD;
	return *this;
}


AosTimeDuration 
AosTimeDuration::operator + (const AosTimeDuration &rhs)
{
	return AosTimeDuration(mTD + rhs.mTD);
}


AosTimeDuration 
AosTimeDuration::operator - (const AosTimeDuration &rhs)
{
	return AosTimeDuration(mTD - rhs.mTD);
}


bool
AosTimeDuration::operator == (const AosTimeDuration &rhs)
{
	return mTD == rhs.mTD;
}


bool 
AosTimeDuration::operator != (const AosTimeDuration &rhs)
{
	return mTD != rhs.mTD;
}


bool 
AosTimeDuration::operator < (const AosTimeDuration &rhs)
{
	return mTD < rhs.mTD;
}


bool 
AosTimeDuration::operator <= (const AosTimeDuration &rhs)
{
	return mTD <= rhs.mTD;
}


bool 
AosTimeDuration::operator >  (const AosTimeDuration &rhs)
{
	return mTD > rhs.mTD;
}


bool 
AosTimeDuration::operator >= (const AosTimeDuration &rhs)
{
	return mTD >= rhs.mTD;
}
