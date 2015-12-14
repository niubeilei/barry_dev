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
#ifndef Omn_COunterTime_TimeCond_h
#define Omn_COunterTime_TimeCond_h

#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "CounterTime/Ptrs.h"


class AosTimeCond : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString	mStartTime;
	OmnString 	mEndTime;
	OmnString	mType;

	u32			mStartYear;
	u32			mEndYear;

	int 		mStartMonth;
	int 		mEndMonth;

	int			mStartDay;
	int 		mEndDay;

	int 		mStartHour;
	int			mEndHour;

	int			mStartSpendDay;
	int			mEndSpendDay;

	int 		mStartSpendHour;
	int			mEndSpendHour;

public:
	AosTimeCond(
			const OmnString startTime,
			const OmnString endTime,
			const OmnString type);
	AosTimeCond(
			const long startTime,
			const long endTime,
			const OmnString type);
	AosTimeCond();
	~AosTimeCond() {}
	
	u32 getStartYear() const {return mStartYear;}
	u32 getEndYear() const {return mEndYear;}

	int getStartMonth() const {return mStartMonth;}
	int getEndMonth() const {return mEndMonth;}

	int getStartDay() const {return mStartDay;}
	int getEndDay() const {return mEndDay;}

	int getStartHour() const {return mStartHour;}
	int getEndHour() const {return mEndHour;}

	int getStartSpendDay() const {return mStartSpendDay;}
	int getEndSpendDay() const {return mEndSpendDay;}

	OmnString getType() const {return mType;}


private:
	bool	init();
	bool	initWithNum(const long startTime, const long endTime);
};
#endif

