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
// 06/28/2010: Created by Lynch yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicTypes.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"


extern AosStr2U32_t		sgGicIdMap;
static OmnMutex			sgLock;

AosGicType::E
AosGicType::toEnum(const OmnString &name)
{
	if (name.length() < 1) return eInvalid;

	sgLock.lock();
	if (sgGicIdMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgGicIdMap.find(name);
	sgLock.unlock();

	if (itr == sgGicIdMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosGicType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "GIC Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "GIC has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgGicIdMap[name] = eid;
	sgLock.unlock();
	return true;
}

