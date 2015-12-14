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
// 01/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "ValueSel/VsTypes.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include "ValueSel/AllValueSel.h"

extern AosStr2U32_t 		sgValueSelMap;
static OmnString			sgNames[AosValueSelType::eMax];
static OmnMutex				sgLock;

AosValueSelType::E 
AosValueSelType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (sgValueSelMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgValueSelMap.find(id);
	sgLock.unlock();
	if (itr == sgValueSelMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosValueSelType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Value Selector Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Value Selector has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgValueSelMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}


OmnString  
AosValueSelType::toString(const E type)
{
	if (!isValid(type)) return "";
	return sgNames[type];
}

#endif
