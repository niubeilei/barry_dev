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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryCond/CondIds.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgCondidMap;
static OmnMutex				sgLock;

AosCondId::E 
AosCondId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgCondidMap.find(id);
	sgLock.unlock();
	if (itr == sgCondidMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosCondId::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Cond Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Cond has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgCondidMap[name] = eid;
	sgLock.unlock();
	return true;
}

