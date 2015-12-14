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
#include "Query/QueryType.h"
/*
#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t 		sgQueryIdMap;
static OmnMutex				sgLock;

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
*/
