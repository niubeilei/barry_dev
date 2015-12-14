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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataSamplerType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t			sgNameMap;
static OmnString			sgNames[AosDataSamplerType::eMax];
static OmnMutex				sgLock;
static bool					sgInited = false;


bool
AosDataSamplerType::check()
{
	sgLock.lock();
	if (sgInited) 
	{
		sgLock.unlock();
		return true;
	}

	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toString((E)i);
		if (name == "")
		{
			OmnAlarm << "Data Sampler not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Data cacher not registered correctly: " 
				<< i << ":" << name << enderr;
			continue;
		}
	}

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosDataSamplerType::E 
AosDataSamplerType::toEnum(const OmnString &id)
{
	if (!sgInited) check();

	if (id.length() < 1) return eInvalid;

	AosStr2U32Itr_t itr = sgNameMap.find(id);
	if (itr == sgNameMap.end()) return eInvalid;
	return (E) itr->second;
}


OmnString
AosDataSamplerType::toString(const E id)
{
	if (id <= eInvalid || id >= eMax) return AOSDATASAMPLERTYPE_INVALID;

	if (!sgInited) check();
	return sgNames[id];
}


bool
AosDataSamplerType::addName(const OmnString &name, const E eid)
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
		OmnString errmsg = "Name already added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgNameMap[name] = eid;
	sgNames[eid] = name;
	return true;
}

