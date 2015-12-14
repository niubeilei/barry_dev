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
#if 0
#include "CommandProc/CommandTypes.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t 		sgNameMap;
static OmnMutex				sgLock;

AosCommandType::E 
AosCommandType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (sgNameMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgNameMap.find(id);
	sgLock.unlock();
	if (itr == sgNameMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosCommandType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	sgLock.lock();
	if (ee != eInvalid)
	{
		sgLock.unlock();
		errmsg = "Name has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgNameMap[name] = eid;
	sgLock.unlock();
	return true;
}

#endif
