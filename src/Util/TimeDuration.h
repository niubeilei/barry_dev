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
#ifndef Omn_Util_TimeDuration_h
#define Omn_Util_TimeDuration_h

#include "Util/DataTypes.h"
#include "Util/String.h"
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;

class AosTimeDuration
{
public:
	time_duration 			mTD;
	
public:
	AosTimeDuration(i64 hours = 0, i64 minutes = 0,
			i64 seconds = 0, i64 nanoseconds = 0);
			
	AosTimeDuration(const AosTimeDuration &rhs);
	AosTimeDuration(const time_duration &td);
	~AosTimeDuration();

	AosTimeDuration		fromString(const string &value);
	string	 			toString() const;
	int64_t				totalSeconds() const { return mTD.total_seconds(); }
	int64_t 			hours() const { return mTD.hours(); }	
	int64_t				minutes() const {return mTD.minutes(); }
	int64_t				seconds() const { return mTD.seconds(); }
	int64_t				totalMilliSeconds() const {return mTD.total_milliseconds(); }
	int64_t 			totalMicroSeconds() const { return mTD.total_microseconds(); }
	int64_t 			totalNanoSeconds() const { return mTD.total_nanoseconds(); }
	bool				isNotADateTime() const { return  mTD.is_not_a_date_time(); }

	bool 				operator == (const AosTimeDuration &rhs);
	bool 				operator != (const AosTimeDuration &rhs);
	bool 				operator <  (const AosTimeDuration &rhs);
	bool 				operator <= (const AosTimeDuration &rhs);
	bool 				operator >  (const AosTimeDuration &rhs);
	bool 				operator >= (const AosTimeDuration &rhs);

	AosTimeDuration& 	operator = (const AosTimeDuration &rhs);
	AosTimeDuration 	operator + (const AosTimeDuration &rhs);
	AosTimeDuration 	operator - (const AosTimeDuration &rhs);

private:

};

#endif // Omn_Util_TimeDuration_h
