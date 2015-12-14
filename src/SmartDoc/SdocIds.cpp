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
// 07/23/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocIds.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t 		sgIdMap;
static OmnMutex				sgLock;

AosSdocId::E 
AosSdocId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (sgIdMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgIdMap.find(id);
	sgLock.unlock();
	if (itr == sgIdMap.end()) 
	{
		return eInvalid;
	}
	return (E) itr->second;
}


bool
AosSdocId::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Name is empty!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Name already being used: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgIdMap[name] = eid;
	sgLock.unlock();
	return true;
}

