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
// This class is the semantics object for classes. It provides the facilities
// that are needed for class semantic rules. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticData/ClassObj.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "Util1/Time.h"



AosClassObj::AosClassObj(const std::string &name)
:
AosVar(name)
{
}


AosClassObj::~AosClassObj()
{
}


// 
// The function retrieves the number of instances created
// in the time range [time_min, time_max], or determined
// by 'time_measure'. 
//
int64_t 	
AosClassObj::getInstCreated(const int time_min, 
						    const int time_max,
						    const AosTimeMeasure::E time_measure)
{
	// 
	// The fu
	// The class maintains the statistics on the number of 
	// instances created in the past at per second resolution.
	// 
	u64 crtSec = OmnTime::getCrtSec();
	aos_assert_r(time_min >= 0 && (u32)time_min <= crtSec, 0);
	aos_assert_r(time_max >= 0 && (u32)time_max <= crtSec, 0);
	aos_assert_r(time_min <= time_max, 0);

	u32 startTime, endTime;
	switch (time_measure)
	{
	case AosTimeMeasure::eOffsetOfCrtTime:	
		 if (time_min == 0 ) 
		 {
			startTime = OmnTime::getStartSec();
		 } 
		 else
		 {
			startTime = crtSec - time_min;
		 }

		 endTime = crtSec - time_max;
		 aos_assert_r(endTime < startTime, 0);
		 break;

	case AosTimeMeasure::eLastInstCreated:
		 startTime = mLastInstCreationTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastInstDeleted:
		 startTime = mLastInstDeletionTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastInstRead:
		 startTime = mLastReadTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastInstModified:
		 startTime = mLastModifyTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastReset:
		 startTime = mLastResetTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastMarker:
		 startTime = mLastMarkerTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastCounter:
		 startTime = mLastCounterTimestamp;
		 endTime = crtSec;
		 break;

	case AosTimeMeasure::eLastPat:
		 startTime = mLastPatTimestamp;
		 endTime = crtSec;
		 break;

	default:
		 OmnAlarm << "Unrecognized time measure: " 
			 << (u32)time_measure << enderr;
		 return 0;
	}

	return getInstCreated(startTime, endTime);
}


int64_t 
AosClassObj::getInstCreated(const u32 startTime, const u32 endTime)
{
	// 
	// Find the end
	//
	std::list<InstCount>::iterator itr;
	int64_t count = 0;
	for (itr = mInstStatList.end(); itr != mInstStatList.begin(); itr++)
	{
		if (itr->mSec < startTime) return count;
		if (itr->mSec > endTime) continue;
		count += itr->mCount;
	}
	return count;
}

