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
#ifndef AOS_TaskUtil_TaskStatus_h
#define AOS_TaskUtil_TaskStatus_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/String.h"


#define AOSTASKSTATUS_INVALID 		"invalid"
#define AOSTASKSTATUS_STOP			"stop"
#define AOSTASKSTATUS_START			"start"
#define AOSTASKSTATUS_FAIL			"fail"
#define AOSTASKSTATUS_PAUSE			"pause"
#define AOSTASKSTATUS_FINISH		"finish"
#define AOSTASKSTATUS_WAIT			"wait"

class AosTaskStatus
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
		case eStop:		return AOSTASKSTATUS_STOP;
		case eStart:	return AOSTASKSTATUS_START;
		case eFail:		return AOSTASKSTATUS_FAIL;
		case ePause:	return AOSTASKSTATUS_PAUSE;
		case eFinish:	return AOSTASKSTATUS_FINISH;
		case eWait:		return AOSTASKSTATUS_WAIT;
		default:		return AOSTASKSTATUS_STOP; 
		}
		OmnAlarm << "Unrecognized status code: " << code << enderr;
		return AOSTASKSTATUS_INVALID;
	}

	static E toEnum(const OmnString &str)
	{
		if (str == AOSTASKSTATUS_STOP) return eStop;
		if (str == AOSTASKSTATUS_START) return eStart;
		if (str == AOSTASKSTATUS_FAIL) return eFail;
		if (str == AOSTASKSTATUS_PAUSE) return ePause;
		if (str == AOSTASKSTATUS_FINISH) return eFinish;
		if (str == AOSTASKSTATUS_WAIT) return eWait;
		return eInvalid;
	}


	inline static bool isFinished(const AosTaskStatus::E code)
	{
		return code == eFinish;
	}

	inline static bool isFailed(const AosTaskStatus::E code)
	{
		return code == eFail;
	}

	inline static bool isValid(const AosTaskStatus::E code)
	{
		return code > eInvalid && code < eMax;
	}
};

#endif
