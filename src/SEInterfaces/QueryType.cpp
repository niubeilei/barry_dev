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
// 08/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t 		sgQueryIdMap;
static OmnMutex				sgLock;
static bool					sgInited = false;


bool 
AosQueryType::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	sgQueryIdMap[AOSTERMTYPE_JIMO] = eJimo;
	sgInited = true;
	sgLock.unlock();
	return true;
}


// Chen Ding, 2013/12/06
OmnString
AosQueryType::toStr(const E code)
{
	sgLock.lock();
	AosStr2U32Itr_t itr = sgQueryIdMap.begin();
	while (itr != sgQueryIdMap.end())
	{
		if (itr->second == code) return itr->first;
		itr++;
	}
	sgLock.unlock();
	return "NotFound";
}


AosQueryType::E 
AosQueryType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (sgQueryIdMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgQueryIdMap.find(id);
	sgLock.unlock();
	if (itr == sgQueryIdMap.end()) 
	{
		// Chen Ding, 08/23/2011
		// Backward Compatibility
		if (id == "eq")
		{
			return eArith;
		}
		return eInvalid;
	}
	return (E) itr->second;
}


bool
AosQueryType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (!sgInited) init();

	if (name == "")
	{
		errmsg = "Query Term Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Query Term has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgQueryIdMap[name] = eid;
	sgLock.unlock();
	return true;
}

