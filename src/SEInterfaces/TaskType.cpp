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
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgTaskTypeMap;
static OmnMutex				sgLock;
OmnString AosTaskType::smNames[AosTaskType::eMax];

AosTaskType::E 
AosTaskType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgTaskTypeMap.find(id);
	sgLock.unlock();
	if (itr == sgTaskTypeMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosTaskType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Task Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Task has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgTaskTypeMap[name] = eid;
	smNames[eid] = name;
	sgLock.unlock();
	return true;
}

