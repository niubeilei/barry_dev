////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 07/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JobStatus_h
#define AOS_SEInterfaces_JobStatus_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/String.h"

#define AOSJOBSTATUS_WAIT                   "wait"
#define AOSJOBSTATUS_START                  "start"
#define AOSJOBSTATUS_STOP                   "stop"
#define AOSJOBSTATUS_PAUSE                  "pause"
#define AOSJOBSTATUS_FAIL                   "fail"
#define AOSJOBSTATUS_FINISH                 "finish"
#define AOSJOBSTATUS_INVALID                "invalid"

class AosJobStatus
{
public:
	enum E
	{
		eInvalid,

		eStop,
		eStart,
		eFail,
		ePause,
		eFinish,
		eWait,

		eMax
	};

	static OmnString toStr(const E code)
	{
		switch (code)
		{
		case eStop:		return AOSJOBSTATUS_STOP;
		case eStart:	return AOSJOBSTATUS_START;
		case eFail:		return AOSJOBSTATUS_FAIL;
		case ePause:	return AOSJOBSTATUS_PAUSE;
		case eFinish:	return AOSJOBSTATUS_FINISH;
		case eWait:		return AOSJOBSTATUS_WAIT;
		default: 		return AOSJOBSTATUS_STOP;
		}
		OmnAlarm << "Unrecognized status code: " << code << enderr;
		return AOSJOBSTATUS_INVALID;
	}

	inline static bool isFinished(const AosJobStatus::E code)
	{
		return code == eFinish;
	}

	inline static bool isFailed(const AosJobStatus::E code)
	{
		return code == eFail;
	}

	inline static bool isValid(const AosJobStatus::E code)
	{
		return code > eInvalid && code < eMax;
	}
};

#endif
