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
//
// Modification History:
// 06/07/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_COunterTime_CounterTime_h
#define Omn_COunterTime_CounterTime_h

#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "CounterTime/Ptrs.h"


class AosCounterTime : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
	{
		eInvalid,

		eNoTime,
		eYear,
		eMonth,
		eDay,
		eHour,
		eMinute,
		eSecond,
		eMsecond,

		eMaxFormat
	};

private:
	u32			mYear;
	int 		mMonth;
	int			mDay;
	int			mHour;
	int			mSpendDay;
	int			mSpendHour;
	int			mMinute;
	int			mSecond;
	int			mMsec;

public:
	AosCounterTime(
			const u32 year,
			const int month,
			const int day);
	AosCounterTime(
			const u32 year,
			const int month,
			const int day,
			const int hour);
	AosCounterTime(const OmnString timeStr);
	AosCounterTime(const long time);
	AosCounterTime(const int year, const int month, const int allDay);
	AosCounterTime(const int year, const int month, const int allDay, const int allHour);
	AosCounterTime();

	~AosCounterTime() {}
	
	u32 getYear() const {return mYear;}
	int getMonth() const {return mMonth;}
	int getDay() const {return mDay;}
	int getHour() const {return mHour;}
	int getSpendDay() const {return mSpendDay;}
	int getSpendHour() const {return mSpendHour;}
	OmnString createTimeCounterName(const OmnString &type) const;
	u64 getUniTime(const E time_type, const OmnString &timestr) const;
	static E toEnum(const OmnString &format);

private:
	bool init(const OmnString timeStr);
	bool initWithNum(const long time);

};
#endif

#endif

