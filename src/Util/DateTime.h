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
#ifndef Omn_Util_DateTime_h
#define Omn_Util_DateTime_h

#include "Util/TimeDuration.h"
#include "Util/DataTypes.h"
#include "Util/String.h"
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;

class AosDateTime
{
private:
	string 					mFormat;
	ptime					mPtime;
	
public:
	AosDateTime();
	AosDateTime(const ptime &time, const string &format);
	AosDateTime(const string &value, const string &format);
	AosDateTime(const time_t tt, const string &format);
	AosDateTime(const AosDateTime &rhs);
	~AosDateTime();

	time_t 			to_time_t() const;
	bool			isNotADateTime() const {return mPtime.is_not_a_date_time();}
	string	 		toString() const;
	string	 		toString(const string &fmt) const;
	string			getType() const { return mFormat; }
	bool 			setFormat(const OmnString &fmt);
	string 			getFormat() const;
	bool 			operator == (const AosDateTime &rhs) const;
	bool 			operator != (const AosDateTime &rhs) const;
	bool 			operator <  (const AosDateTime &rhs) const;
	bool 			operator <= (const AosDateTime &rhs) const;
	bool 			operator >  (const AosDateTime &rhs) const;
	bool 			operator >= (const AosDateTime &rhs) const;

	int64_t 		operator - (const AosDateTime &rhs);
	AosDateTime		operator + (const AosTimeDuration &td);
	AosDateTime		operator - (const AosTimeDuration &td);
	AosDateTime& 	operator = (const AosDateTime &rhs);

	ptime& 			getPtime() {return mPtime;}


private:
	//struct tm 		toTM() const { return to_tm(mPtime); }
};

#endif // Omn_Util_DateTime_h
