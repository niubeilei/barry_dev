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
// 04/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Groupby/GroupbyType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgGroupbyTypeMap;
static OmnString			sgNames[AosGroupbyType::eMax];
static OmnMutex				sgLock;
static bool					sgChecked = false;
static bool					sgInited = false;


bool
AosGroupbyType::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = sgNames[i];
		if (name == "")
		{
			OmnAlarm << "Not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Not registered correctly: " << i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


bool
AosGroupbyType::init()
{
	sgLock.lock();
	if (!sgInited)
	{
		sgGroupbyTypeMap[AOSGROUPBYTYPE_NORM] = eNorm;
		sgNames[eNorm] = AOSGROUPBYTYPE_NORM;

	}

	sgInited = true;
	sgLock.unlock();

	if (!check())
	{
		OmnAlarm << "Failed init groupby type" << enderr;
	}
	return true;
}


AosGroupbyType::E 
AosGroupbyType::toEnum(const OmnString &id)
{
	if (!sgInited) init();

	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgGroupbyTypeMap.find(id);
	sgLock.unlock();
	if (itr == sgGroupbyTypeMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosGroupbyType::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		OmnString errmsg = "Name has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgGroupbyTypeMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}

