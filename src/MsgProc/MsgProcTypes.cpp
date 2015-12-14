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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MsgProc/MsgProcTypes.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MsgProc/MsgProc.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t         sgNameMap;
static OmnString			sgNames[AosMsgProcType::eMax];
static OmnMutex             sgLock;
static bool					sgInited = false;

AosMsgProcType::E 
AosMsgProcType::toEnum(const OmnString &name)
{
	if (!sgInited) 
	{
		AosMsgProc::initAllProcs();
	}

	if (name.length() < 1) return eInvalid;
	
	if (sgNameMap.empty()) return eInvalid;
	AosStr2U32Itr_t itr = sgNameMap.find(name);
	if (itr == sgNameMap.end()) return eInvalid;
	return (E) itr->second;
}


OmnString 
AosMsgProcType::toString(const E code)
{
	if (!sgInited) AosMsgProc::initAllProcs();
	if (!isValid(code)) return AOSMSGPROCTYPE_INVALID;
	return sgNames[code];
}


bool
AosMsgProcType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgInited = true;
	sgNameMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}


