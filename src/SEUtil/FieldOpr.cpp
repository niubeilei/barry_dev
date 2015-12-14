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
// 05/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/FieldOpr.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgFieldOprMap;
static OmnString			sgNames[AosFieldOpr::eMaxEntry];
static OmnMutex				sgLock;
static bool 				sgInited = false;

bool
AosFieldOpr::init()
{
	static OmnMutex lsLocalLock;

	lsLocalLock.lock();
	addName(AOSDBOPR_FIRST,		eFirst);
	addName(AOSDBOPR_LAST,		eLast);
	addName(AOSDBOPR_SUM, 		eSum);
	addName(AOSDBOPR_MIN, 		eMinimum);
	addName(AOSDBOPR_MAX,		eMaximum);
	addName(AOSDBOPR_AVERAGE,	eAverage);
	addName(AOSDBOPR_COUNT,		eCount);
	addName(AOSDBOPR_FIELD,		eField);
	addName(AOSDBOPR_NONE,		eNone);

	sgInited = true;
	for (int i=eInvalid+1; i<eMaxEntry; i++)
	{
		OmnString name = toStr((E)i);
		E code = toEnum(name);
		if (code != (E)i)
		{
			OmnAlarm << "DBOperator not registered yet: " << name << enderr;
		}
	}
	lsLocalLock.unlock();
	return true;
}


OmnString
AosFieldOpr::toStr(const E id)
{
	if (id <= eInvalid || id >= eMaxEntry) return AOSDBOPR_INVALID;

	if (!sgInited) init();
	return sgNames[id];
}


AosFieldOpr::E 
AosFieldOpr::toEnum(const OmnString &id)
{
	if (!sgInited) init();

	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgFieldOprMap.find(id);
	sgLock.unlock();
	if (itr == sgFieldOprMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosFieldOpr::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgFieldOprMap[name] = eid;
	sgNames[eid] = name;
	return true;
}

