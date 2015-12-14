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
// This schedule is configured with a specific time, such as:
// 	"02:30":			2:30, today
// 	"2013/10/25 02:30"	2:30 on 2013/10/25
//
// When the time comes, it runs the runnable. 
//
// Modification History:
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Scheduler/Jimos/ScheduleTime.h"

#include "Scheduler/Scheduler.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosScheduleTime_0(
 		const AosRundataPtr &rdata, 
 		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosScheduleTime(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


// Chen Ding, 2014/01/12
// AosScheduleTime::AosScheduleTime(
// 		const AosRundataPtr &rdata,
// 		const AosXmlTagPtr &def)
AosScheduleTime::AosScheduleTime(const u32 version)
:
AosSchedule(rdata, def),
mLastRunTime(0),
mStatus(eIdle),
mNumRuns(0)

{
	if (!config(rdata, def)) OmnThrowException(rdata->getErrmsg());
}


AosScheduleTime::~AosScheduleTime()
{
}


bool
AosScheduleTime::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// 'def' is in the following format:
	// 	<schedule time="ddd" 
	// 		repeat="hourly|daily|minutely" .../>
	//
	// the format of schedule time is considered the following:
	//
	//	* 		* 	  * 	* 		* 	 
	// minute  hour  day  month  dayofweek  
	//	00 		21 	  2 	6 		1 	 
	// minute  hour  day  month  dayofweek  
	
	
	
	if (!def) return true;
	mMinInterval = def->getAttrInt("mininterval", 1);
	aos_assert_r(mMinInterval > 0, false);

	mRepeatTimes = def->getAttrInt("repeattimes", -1);

	OmnString time_str = def->getAttrStr("time", "");
	aos_assert_r(time_str != "", false);
	
	aos_assert_r(parseTime(time_str), false);

	// Currently, we support:
	// 	"hourly"
	// 	"daily"
	// 	"minutely"
	// 	"dayofweek"
	//	"seconds"	(every N seconds)
	mRepeat = def->getAttrStr("repeat", "");
	if (mRepeat == "seconds")
	{
		mSeconds = def->getAttrInt("repeat_seconds", -1);
		if (mSeconds <= 0)
		{
			OmnAlarm << "Invalid seconds: " << def->toString() << enderr;
			return false;
		}
	}

	return true;
}



AosJimoPtr 
AosScheduleTime::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosScheduleTime(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosScheduleTime::readyToRun(AosRundataPtr &rdata) 
{
	// This function checks whether the scheduled time has arrived.
	// If yes and if the time since the last launch time is greater
	// than a given time, it is ready to run.
	//
	
	if(mFinished) return false;
	if (mStatus != eIdle) return false;

	u32 crt_time = OmnGetSecond();
	if (crt_time < mExpectedTime) return false;

	if (mLastRunTime > 0)
	{
		u64 tt = OmnGetTimestamp();
		aos_assert_rr(tt >= mLastRunTime, rdata, false);
		if (tt - mLastRunTime < mMinInterval) return false;
	}

	return true;
}


bool
AosScheduleTime::runnableStarted()
{
	mStatus = eActive;
	mNumRuns ++;

	mLastRunTime = OmnGetTimestamp();

	if (mRepeat == "seconds")
	{
		aos_assert_r(mSeconds > 0, false);
		mExpectedTime += mSeconds;
		return true;
	}

	if (mRepeat == "hourly")
	{
		mExpectedTime += 60*60;
		u64 crt_time = OmnGetSecond();
		while (mExpectedTime <= crt_time)
		{
			mExpectedTime += 60*60;
		}
		return true;
	}

	if (mRepeat == "strict_hourly")
	{
		mExpectedTime += 60*60;
		return true;
	}

	if (mRepeat == "daily")
	{
		mExpectedTime += 60*60*24;
		return true;
	}

	return true;
}


bool
AosScheduleTime::setRunnableFinished()
{
	mStatus = eIdle;
	if(mRepeatTimes > 0)
	{
		if(mNumRuns >= mRepeatTimes) 
		{
			//run more finished 
			mFinished = true;
		}
	}
	if(mRepeat == "")
	{
		//run once finished
		mFinished = true;
	}
	return true;
}

bool
AosScheduleTime::parseTime(OmnString &timestr)
{

	OmnStrParser1 parser(timestr, " ");
	parser.reset();
	int count = 0;
	int looponly = 0;
	while (parser.hasMore())
	{
		if(count == 0)
		{
			mMinuteMark = parser.nextWord();
			OmnString star = mMinuteMark.substr(0,1);
			if(star == "*")
			{
				if(mMinuteMark.length() > 1)
				{
					mReapeat = "minutely";
					OmnString looptime = mMinuteMark.substr(2,mMinuteMark.length()-1);
					mLoopTime = atoi(looptime.data());
					looponly++;
				}
				else
				{
					mMinuteMark = mMinuteMark.substr(2,mMinuteMark.length()-1);
				}
			}
		}
		if(count == 1)
		{
			mHourMark = parser.nextWord();
			OmnString star = mHourMark.substr(0,1);
			if(star == "*")
			{
				if(mHourMark.length() > 1)
				{
					mReapeat = "hourly";
					OmnString looptime = mHourMark.substr(2,mHourMark.length()-1);
					mLoopTime = atoi(looptime.data());
					looponly++;
				}
				else
				{
					mHourMark = mHourMark.substr(2,mHourMark.length()-1);
				}
			}
		}
		if(count == 2)
		{
			mDayMark = parser.nextWord();
			OmnString star = mDayMark.substr(0,1);
			if(star == "*")
			{
				if(mDayMark.length() > 1)
				{
					mReapeat = "daily";
					OmnString looptime = mDayMark.substr(2,mDayMark.length()-1);
					mLoopTime = atoi(looptime.data());
					looponly++;
				}
				else
				{
					mDayMark = mDayMark.substr(2,mDayMark.length()-1);
				}
			}
		}
		if(count == 3)
		{
			mMonthMark = parser.nextWord();
			OmnString star = mMonthMark.substr(0,1);
			if(star == "*")
			{
				if(mMonthMark.length() > 1)
				{
					mReapeat = "daily";
					OmnString looptime = mMonthMark.substr(2,mMonthMark.length()-1);
					mLoopTime = atoi(looptime.data());
					looponly++;
				}
				else
				{
					mMonthMark = mMonthMark.substr(2,mMonthMark.length()-1);
				}
			}
		}
		if(count == 4)
		{
			mDayOfWeekMark = parser.nextWord();
			if( mDayOfWeekMark != "*")
			{
				mReapeat = "dayofweekly";
				mLoopTime = atoi(mDayOfWeekMark.data());
				looponly++;
			}
		}

		count++;
	}
	
	if(count != 5)
	{
		return false;
	}

	if(looponly>1)
	{
		return false;
	}

	mExpectedTime = OmnGetSecond();

	return true;

}

bool 
AosScheduleTime::scheduleFinished()
{
	return mFinished;
}
