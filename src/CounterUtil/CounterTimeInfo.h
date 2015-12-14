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
// This is a utility class.  
//
// Modification History:
// 02/28/2013: Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_CounterTimeInfo_h
#define Aos_CounterUtil_CounterTimeInfo_h

#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"
#include "UtilTime/TimeUtil.h"

class AosCounterTimeInfo
{
	OmnDefineRCObject;
private:
	u64                     start_time;
	u64                     end_time;
	AosTimeGran::E          time_gran;
	AosTime::TimeFormat     time_format;
	bool					use_epochtime;
	OmnString				time_opr;

public:
	AosCounterTimeInfo()
	:
	start_time(AosTime::eInvalidTime),
	end_time(AosTime::eInvalidTime),
	time_gran(AosTimeGran::eNoTime)
	{
	}

	bool parseTime(const AosXmlTagPtr &tag, const OmnString &tagname, const AosRundataPtr &rdata)
	{
		aos_assert_rr(tag, rdata, false);
		aos_assert_rr(tagname != "", rdata, false);
		AosXmlTagPtr child = tag->getFirstChild(tagname);
		if (!child) return true;

		use_epochtime = child->getNodeText("use_epochtime") == "true"?true:false;

		OmnString timegran = child->getNodeText(AOSTAG_TIMEGRAN);
		if (timegran == "") timegran = "ntm";
		time_gran = AosTimeGran::toEnum(timegran);
		aos_assert_r(AosTimeGran::isValid(time_gran), false);

		time_format = AosTime::convertTimeFormatWithDft(child->getAttrStr("zky_timeformat", "2fmt"));

		OmnString stimeStr = child->getNodeText("zky_starttime");
		OmnString etimeStr = child->getNodeText("zky_endtime");
		if (stimeStr == "" || etimeStr == "")
		{
			int num = child->getAttrInt("zky_timenumber", -1);
			OmnString type = child->getNodeText("zky_timetype"); 
			if (type == "") return true;
			//y,m,w,d,H,M,S
			bool rslt = AosTime::calculateTime(type, num, stimeStr, etimeStr, rdata);
			aos_assert_r(rslt, false);
		}

		i64 startime, endtime;
		if (use_epochtime)
		{
			startime = AosTimeUtil::str2EpochTime(stimeStr);
			endtime = AosTimeUtil::str2EpochTime(etimeStr);
		}
		else
		{
			startime = AosTime::getUniTime(time_format, AosTimeGran::eSecondly, stimeStr, "%Y-%m-%d %H:%M:%S");
			endtime = AosTime::getUniTime(time_format, AosTimeGran::eSecondly, etimeStr, "%Y-%m-%d %H:%M:%S");
		}

		if (time_gran != AosTimeGran::eNoTime)
		{
			if (startime != 0 && endtime != 0 && startime > endtime)
			{
				OmnAlarm << "start_time(" << startime << ") > end_time(" << endtime << ")"<< enderr;
			}
			start_time = startime;
			end_time = endtime;
		}
		return true;
	}

	bool isValidTimeRange()
	{
		return AosTime::isValidTimeRange(start_time, end_time);
	}

	u64 convertUniTime(const u64 &time)
	{
		return AosTime::convertUniTime(time_format, time_gran, time, use_epochtime);
	}

	u64 nextTime(const u64 &time)
	{
		return  AosTime::nextTime(time, time_gran, time_format, use_epochtime);
	}

	AosTimeGran::E getTimeGran() const
	{
		return time_gran;
	}

	u64 getStartTime() const
	{
		return start_time;
	}

	u64 getEndTime() const
	{
		return end_time;
	}
};
#endif

