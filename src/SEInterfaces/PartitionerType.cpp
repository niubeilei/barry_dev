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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/PartitionerType.h"

#include "alarm_c/alarm.h"
#include "Debug/ExitHandler.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgPartitionerMap;
static OmnString			sgNames[AosPartitionerType::eMax];
static OmnMutex				sgLock;
bool AosPartitionerType::smInited = false;


bool
AosPartitionerType::check()
{
	bool rslt = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = sgNames[i];
		if (name == "")
		{
			rslt = false;
			OmnAlarm << "Partitioner not registered: " << i << enderr;
		}

		E code = toEnum(name);
		if (code != (E)i)
		{
			rslt = false;
			OmnAlarm << "Partitioner registered incorrect: " << code << ":" << i << enderr;
		}
	}
	return rslt;
}

		
AosPartitionerType::E 
AosPartitionerType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (!smInited)
	{
		sgLock.unlock();
		OmnAlarm << "Partitioners were not initialized. Please add the following"
			<< " line in main.cpp:\n"
			<< "      AosPartitioner::init();" << enderr;
		OmnExitApp("Partitioners not initialized");
		return eInvalid;
	}

	AosStr2U32Itr_t itr = sgPartitionerMap.find(id);
	sgLock.unlock();
	if (itr == sgPartitionerMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosPartitionerType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Partitioner Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Partitioner has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgPartitionerMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}


OmnString  
AosPartitionerType::toString(const E type)
{
	if (!isValid(type)) return "";
	return sgNames[type];
}


