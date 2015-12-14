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
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Scheduler_JimoScheduleTime_h
#define Aos_Scheduler_JimoScheduleTime_h

#include "Rundata/Rundata.h"
#include "Jimo/Jimo.h"
#include "Scheduler/Schedule.h"
#include "Util/ValueRslt.h"

#include "SEInterfaces/Ptrs.h"

class AosScheduleTime : public AosSchedule
{
	OmnDefineRCObject;

	enum
	{
		eIdle,
		eActive
	};

private:
	OmnString 				mTime;
	OmnString 				mRepeat;
	u32						mExpectedTime;
	u64						mLastRunTime;
	int						mStatus;
	int						mSeconds;
	u64						mMinInterval;
	int						mRepeatTimes;
	int						mNumRuns;
	int						mLoopTime;

	OmnString				mReapeat;
	OmnString				mMinuteMark;
	OmnString				mHourMark;
	OmnString				mDayMark;
	OmnString				mMonthMark;
	OmnString				mDayOfWeekMark;

	bool parseTime(OmnString &time);

public:
	AosScheduleTime(){}
	AosScheduleTime(const AosRundataPtr &rdata, 
					const AosXmlTagPtr &sdoc);
	~AosScheduleTime();

	// Jimo Interface
//	virtual bool run(const AosRundataPtr &rdata);
//	virtual bool run(const AosRundataPtr &rdata, const AosXmlTagPtr &xml);
	virtual AosJimoPtr cloneJimo(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc)  const;
	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc);
	
	// Schedule Interface
	virtual bool scheduleFinished();
	virtual bool readyToRun(AosRundataPtr &rdata);
	virtual bool runnableStarted();
	virtual bool setRunnableFinished();

};
#endif
