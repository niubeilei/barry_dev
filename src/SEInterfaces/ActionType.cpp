////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ActionType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include <vector>
using namespace std;

static AosStr2U32_t			sgIdMapAct;
static OmnString			sgNames[AosActionType::eMax];
static OmnMutex				sgLock;
static OmnMutex				sgCheckLock;
static bool 				sgInited = false;


bool
AosActionType::check()
{
	if (sgInited) return true;
	sgCheckLock.lock();
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toString((E)i);
		E code = toEnum(name);
		if (code != i)
		{
//			OmnAlarm << "Action not registered: " << i << enderr;
		}
	}

	// Chen Ding, 12/25/2012
	sgInited = true;
	sgCheckLock.unlock();
	return true;
}


OmnString
AosActionType::toString(const E code)
{
	if (code <= eInvalid || code >= eMax) return "invalid";
	return sgNames[code];
}


AosActionType::E 
AosActionType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgIdMapAct.find(id);
	sgLock.unlock();
	if (itr == sgIdMapAct.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosActionType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "SeRequest Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "SeRequest has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgIdMapAct[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}

