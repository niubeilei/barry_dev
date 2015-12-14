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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskDataType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgTaskDataTypeMap;
static AosU322Str_t			sgTaskDataNameMap;
static OmnMutex				sgLock;


AosTaskDataType::E 
AosTaskDataType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgTaskDataTypeMap.find(id);
	sgLock.unlock();
	if (itr == sgTaskDataTypeMap.end()) return eInvalid;
	return (E) itr->second;
}
	

OmnString
AosTaskDataType::toStr(const AosTaskDataType::E id)
{
	if (id <= eInvalid || id >= eMax) return AOSTASKDATA_INVALID;
	
	sgLock.lock();
	AosU322StrItr_t itr = sgTaskDataNameMap.find(id);
	sgLock.unlock();
	if (itr == sgTaskDataNameMap.end()) return AOSTASKDATA_INVALID;
	return (OmnString) itr->second;
}


bool
AosTaskDataType::addName(
		const OmnString &name,
		const E eid,
		OmnString &errmsg)
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
	sgTaskDataTypeMap[name] = eid;
	sgTaskDataNameMap[eid] = name;
	sgLock.unlock();
	return true;
}

