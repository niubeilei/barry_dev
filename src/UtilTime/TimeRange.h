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
//	07/29/2011 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_TimeRange_h
#define Omn_UtilTime_TimeRange_h

#include "aosUtil/Types.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "UtilTime/TimeInfo.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/Ptrs.h"
#include "Rundata/Rundata.h"


class AosTimeRange
{
private:
	AosTime		mStartTime;
	AosTime		mEndTime;

public:
	AosTimeRange(){}
	AosTimeRange(
			const OmnString &startTime,
			const OmnString &endTime);
	AosTimeRange(
			const u64 &startTime,
			const u64 &endTime);
	AosTimeRange(const AosXmlTagPtr &cond, const AosRundataPtr &rdata);
	~AosTimeRange() {}
	
	u64 getStartUniTime() const;
	u64 getEndUniTime() const;
	bool isValid() const
	{
		return mStartTime.isValid() || mEndTime.isValid();
	}

	static bool getTime(const AosCalendarPtr &calendar, 
			const OmnString &timestr, 
			const OmnString &timeformat, 
			AosTimeRange &timeinfo,
			const int start_time, 
			const int stop_end, 
			const int begin_time,
			const int end_time,
			const AosRundataPtr &rdata);
};
#endif

