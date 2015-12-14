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
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskTransType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgTaskTransTypeMap;
static OmnMutex				sgLock;
OmnString AosTaskTransType::smNames[AosTaskTransType::eMax];

AosTaskTransType::E 
AosTaskTransType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgTaskTransTypeMap.find(id);
	sgLock.unlock();
	if (itr == sgTaskTransTypeMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosTaskTransType::addName(const OmnString &name, const E eid, OmnString &errmsg)
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
	sgTaskTransTypeMap[name] = eid;
	smNames[eid] = name;
	sgLock.unlock();
	return true;
}

