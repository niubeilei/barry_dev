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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogClient/Tester/Reqids.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgReqidMap;
static OmnMutex				sgLock;

AosLogReqid::E 
AosLogReqid::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgReqidMap.find(id);
	sgLock.unlock();
	if (itr == sgReqidMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosLogReqid::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "LogTestRequest Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "LogTestRequest has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgReqidMap[name] = eid;
	sgLock.unlock();
	return true;
}

