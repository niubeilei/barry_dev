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
// 07/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataFieldType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t			sgNameMap;
static OmnString			sgNames[AosDataFieldType::eMax];
static OmnMutex				sgLock;
static bool					sgInited = false;


bool
AosDataFieldType::check()
{
	sgLock.lock();
	if (sgInited) 
	{
		sgLock.unlock();
		return true;
	}

	sgInited = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toString((E)i);
		if (name == "")
		{
			OmnScreen << "Data Field not registered: " << i << endl;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnScreen << "Data cacher not registered correctly: " 
				<< i << ":" << name << endl;
			continue;
		}
	}

	sgLock.unlock();
	return true;
}


AosDataFieldType::E 
AosDataFieldType::toEnum(const OmnString &id)
{
	OmnString strtype = id;
	if (strtype.toLower() == "string") strtype = "str";

	if (!sgInited) check();

	if (strtype.length() < 1) return eInvalid;

	AosStr2U32Itr_t itr = sgNameMap.find(strtype);
	if (itr == sgNameMap.end()) return eInvalid;
	return (E) itr->second;
}


OmnString
AosDataFieldType::toString(const E id)
{
	if (id <= eInvalid || id >= eMax) return AOSDATAFIELDTYPE_INVALID;

	//if (!sgInited) check();
	return sgNames[id];
}


bool
AosDataFieldType::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!isValid(eid))
	{
		OmnString errmsg = "Value invalid: ";
		errmsg << name << ":" << eid;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgNameMap[name] = eid;
	sgNames[eid] = name;
	return true;
}


